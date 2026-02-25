using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;

namespace RPGCreator.Core.RPG;

public enum EventTriggerType
{
    Action,
    Autorun,
    Parallel,
    PlayerTouch
}

public enum QuestStatus
{
    Inactive,
    Active,
    Completed,
    Failed
}

public enum ItemType
{
    Consumable,
    Equipment,
    KeyItem
}

public sealed class Tile
{
    public int TileId { get; set; }
    public bool Passable { get; set; } = true;
}

public sealed class TileLayer
{
    private readonly Tile[,] _tiles;

    public TileLayer(int width, int height, string name)
    {
        Name = name;
        Width = width;
        Height = height;
        _tiles = new Tile[width, height];

        for (var x = 0; x < width; x++)
        {
            for (var y = 0; y < height; y++)
            {
                _tiles[x, y] = new Tile();
            }
        }
    }

    public string Name { get; }
    public int Width { get; }
    public int Height { get; }

    public Tile GetTile(int x, int y) => _tiles[x, y];
}

public sealed class Map
{
    public Map(string id, string name, int width, int height)
    {
        Id = id;
        Name = name;
        Width = width;
        Height = height;
    }

    public string Id { get; }
    public string Name { get; }
    public int Width { get; }
    public int Height { get; }
    public List<TileLayer> Layers { get; } = new();
    public List<GameEvent> Events { get; } = new();

    public bool IsPassable(int x, int y)
    {
        if (x < 0 || y < 0 || x >= Width || y >= Height)
        {
            return false;
        }

        // All layers must be passable at this position.
        return Layers.All(layer => layer.GetTile(x, y).Passable);
    }
}

public sealed class MapManager
{
    private readonly Dictionary<string, Map> _maps = new(StringComparer.OrdinalIgnoreCase);

    public Map? CurrentMap { get; private set; }

    public void Register(Map map)
    {
        _maps[map.Id] = map;
    }

    public bool TrySwitchTo(string mapId)
    {
        if (!_maps.TryGetValue(mapId, out var map))
        {
            return false;
        }

        CurrentMap = map;
        return true;
    }
}

public sealed class GameSwitch
{
    public required string Name { get; init; }
    public bool Value { get; set; }
}

public sealed class GameVariable
{
    public required string Name { get; init; }
    public int Value { get; set; }
}

public sealed class EventPage
{
    public EventTriggerType TriggerType { get; set; }
    public Func<EventContext, bool>? Condition { get; set; }
    public Action<EventContext>? Command { get; set; }

    public bool CanRun(EventContext context) => Condition?.Invoke(context) ?? true;
}

public sealed class GameEvent
{
    public required string Id { get; init; }
    public int X { get; set; }
    public int Y { get; set; }
    public List<EventPage> Pages { get; } = new();

    public EventPage? ResolveActivePage(EventContext context)
    {
        for (var i = Pages.Count - 1; i >= 0; i--)
        {
            if (Pages[i].CanRun(context))
            {
                return Pages[i];
            }
        }

        return null;
    }
}

public sealed class EventContext
{
    public required Dictionary<string, bool> Switches { get; init; }
    public required Dictionary<string, int> Variables { get; init; }
    public required Party Party { get; init; }
}

public sealed class EventManager
{
    public Dictionary<string, bool> Switches { get; } = new(StringComparer.OrdinalIgnoreCase);
    public Dictionary<string, int> Variables { get; } = new(StringComparer.OrdinalIgnoreCase);

    public void Trigger(GameEvent gameEvent, Party party, EventTriggerType triggerType)
    {
        var context = new EventContext
        {
            Switches = Switches,
            Variables = Variables,
            Party = party
        };

        var page = gameEvent.ResolveActivePage(context);
        if (page is null || page.TriggerType != triggerType)
        {
            return;
        }

        page.Command?.Invoke(context);
    }
}

public sealed class Stat
{
    public required string Id { get; init; }
    public int BaseValue { get; set; }
    public int BonusValue { get; set; }
    public int Value => BaseValue + BonusValue;
}

public sealed class LevelSystem
{
    public int Level { get; private set; } = 1;
    public int Experience { get; private set; }

    public bool AddExperience(int amount)
    {
        Experience += Math.Max(0, amount);

        var needed = ExperienceToNext(Level);
        if (Experience < needed)
        {
            return false;
        }

        Experience -= needed;
        Level++;
        return true;
    }

    private static int ExperienceToNext(int level) => 50 + (level * 25);
}

public sealed class Item
{
    public required string Id { get; init; }
    public required string Name { get; init; }
    public ItemType Type { get; init; }
    public Action<Actor>? UseEffect { get; init; }
}

public sealed class Inventory
{
    private readonly Dictionary<string, int> _counts = new(StringComparer.OrdinalIgnoreCase);

    public IReadOnlyDictionary<string, int> Counts => _counts;

    public void Add(string itemId, int amount = 1)
    {
        if (amount <= 0)
        {
            return;
        }

        _counts[itemId] = _counts.TryGetValue(itemId, out var old) ? old + amount : amount;
    }

    public bool Consume(string itemId, int amount = 1)
    {
        if (!_counts.TryGetValue(itemId, out var old) || old < amount)
        {
            return false;
        }

        var next = old - amount;
        if (next == 0)
        {
            _counts.Remove(itemId);
        }
        else
        {
            _counts[itemId] = next;
        }

        return true;
    }
}

public sealed class Equipment
{
    private readonly Dictionary<string, Item> _slots = new(StringComparer.OrdinalIgnoreCase);

    public IReadOnlyDictionary<string, Item> Slots => _slots;

    public void Equip(string slotName, Item item)
    {
        _slots[slotName] = item;
    }
}

public sealed class Skill
{
    public required string Id { get; init; }
    public required string Name { get; init; }
    public int MpCost { get; init; }
    public Action<Actor, Actor>? Resolve { get; init; }
}

public sealed class Actor
{
    public required string Id { get; init; }
    public required string Name { get; init; }
    public int Hp { get; set; } = 100;
    public int Mp { get; set; } = 20;
    public List<Stat> Stats { get; } = new();
    public Inventory Inventory { get; } = new();
    public Equipment Equipment { get; } = new();
    public List<Skill> Skills { get; } = new();
    public LevelSystem LevelSystem { get; } = new();

    public bool IsAlive => Hp > 0;
}

public sealed class Enemy
{
    public required string Id { get; init; }
    public required string Name { get; init; }
    public int Hp { get; set; } = 40;
    public int Attack { get; set; } = 8;

    public bool IsAlive => Hp > 0;
}

public sealed class Party
{
    public List<Actor> Members { get; } = new();
    public Actor? Leader => Members.FirstOrDefault();
}

public sealed class BattleAction
{
    public required Actor Source { get; init; }
    public Actor? TargetActor { get; init; }
    public Enemy? TargetEnemy { get; init; }
    public Skill? Skill { get; init; }
}

public sealed class BattleManager
{
    public bool ResolveTurn(BattleAction action)
    {
        if (!action.Source.IsAlive)
        {
            return false;
        }

        if (action.Skill is not null && action.TargetActor is not null)
        {
            if (action.Source.Mp < action.Skill.MpCost)
            {
                return false;
            }

            action.Source.Mp -= action.Skill.MpCost;
            action.Skill.Resolve?.Invoke(action.Source, action.TargetActor);
            return true;
        }

        if (action.TargetEnemy is not null)
        {
            action.TargetEnemy.Hp = Math.Max(0, action.TargetEnemy.Hp - 10);
            return true;
        }

        return false;
    }

    public static bool IsVictory(IEnumerable<Enemy> enemies) => enemies.All(e => !e.IsAlive);
    public static bool IsDefeat(Party party) => party.Members.All(m => !m.IsAlive);
}

public sealed class Choice
{
    public required string Text { get; init; }
    public required string NextNodeId { get; init; }
    public Func<EventContext, bool>? Condition { get; init; }
}

public sealed class DialogueNode
{
    public required string Id { get; init; }
    public required string Text { get; init; }
    public List<Choice> Choices { get; } = new();
}

public sealed class Dialogue
{
    public required string Id { get; init; }
    public required string StartNodeId { get; init; }
    public Dictionary<string, DialogueNode> Nodes { get; } = new(StringComparer.OrdinalIgnoreCase);
}

public sealed class DialogueManager
{
    private Dialogue? _current;
    private DialogueNode? _activeNode;

    public DialogueNode? ActiveNode => _activeNode;

    public bool Start(Dialogue dialogue)
    {
        if (!dialogue.Nodes.TryGetValue(dialogue.StartNodeId, out var startNode))
        {
            return false;
        }

        _current = dialogue;
        _activeNode = startNode;
        return true;
    }

    public bool SelectChoice(int index)
    {
        if (_current is null || _activeNode is null)
        {
            return false;
        }

        if (index < 0 || index >= _activeNode.Choices.Count)
        {
            return false;
        }

        var choice = _activeNode.Choices[index];
        return _current.Nodes.TryGetValue(choice.NextNodeId, out _activeNode);
    }
}

public sealed class QuestStep
{
    public required string Description { get; init; }
    public bool IsCompleted { get; set; }
}

public sealed class Quest
{
    public required string Id { get; init; }
    public required string Name { get; init; }
    public QuestStatus Status { get; set; } = QuestStatus.Inactive;
    public List<QuestStep> Steps { get; } = new();

    public bool IsCompleted => Steps.Count > 0 && Steps.All(step => step.IsCompleted);
}

public sealed class QuestManager
{
    private readonly Dictionary<string, Quest> _quests = new(StringComparer.OrdinalIgnoreCase);

    public IReadOnlyCollection<Quest> Quests => _quests.Values;

    public void Register(Quest quest) => _quests[quest.Id] = quest;

    public bool Start(string questId)
    {
        if (!_quests.TryGetValue(questId, out var quest))
        {
            return false;
        }

        quest.Status = QuestStatus.Active;
        return true;
    }

    public void RefreshCompletion(string questId)
    {
        if (!_quests.TryGetValue(questId, out var quest))
        {
            return;
        }

        if (quest.IsCompleted)
        {
            quest.Status = QuestStatus.Completed;
        }
    }
}

public sealed class SaveData
{
    public required string CurrentMapId { get; init; }
    public required List<string> PartyActorIds { get; init; }
    public required Dictionary<string, bool> Switches { get; init; }
    public required Dictionary<string, int> Variables { get; init; }
}

public sealed class SaveManager
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true
    };

    public void Save(string path, SaveData data)
    {
        var json = JsonSerializer.Serialize(data, JsonOptions);
        File.WriteAllText(path, json);
    }

    public SaveData? Load(string path)
    {
        if (!File.Exists(path))
        {
            return null;
        }

        var json = File.ReadAllText(path);
        return JsonSerializer.Deserialize<SaveData>(json, JsonOptions);
    }
}

public sealed class ItemDatabase
{
    private readonly Dictionary<string, Item> _items = new(StringComparer.OrdinalIgnoreCase);

    public void Register(Item item) => _items[item.Id] = item;
    public bool TryGet(string id, out Item? item) => _items.TryGetValue(id, out item);
}

public sealed class SkillDatabase
{
    private readonly Dictionary<string, Skill> _skills = new(StringComparer.OrdinalIgnoreCase);

    public void Register(Skill skill) => _skills[skill.Id] = skill;
    public bool TryGet(string id, out Skill? skill) => _skills.TryGetValue(id, out skill);
}

public interface IPlugin
{
    string Name { get; }
    void Initialize();
}

public sealed class PluginManager
{
    private readonly List<IPlugin> _plugins = new();

    public IReadOnlyList<IPlugin> Plugins => _plugins;

    public void Register(IPlugin plugin)
    {
        _plugins.Add(plugin);
        plugin.Initialize();
    }
}
