using RPGCreator.Core.RPG;
using RPGCreator.SDK;
using RPGCreator.SDK.Assets.Definitions.Maps;
using RPGCreator.SDK.EngineService;
using RPGCreator.SDK.Logging;

namespace RPGCreator.Core.Services;

public sealed class RpgSystemsService : IRpgSystemsService
{
    private readonly ScopedLogger _logger = Logger.ForContext<RpgSystemsService>();
    private readonly MapManager _mapManager = new();
    private readonly EventManager _eventManager = new();
    private readonly QuestManager _questManager = new();
    private readonly SaveManager _saveManager = new();
    private readonly Party _party = new();

    public RpgSystemsService()
    {
        EngineServices.OnceServiceReady<IProjectsManager>(projects =>
        {
            projects.OnProjectOpened += _ => SyncMapsFromAssets();
        });
    }

    public string? CurrentMapId => _mapManager.CurrentMap?.Id;
    public IReadOnlyCollection<string> KnownMapIds => _knownMaps.Keys;

    private readonly Dictionary<string, Map> _knownMaps = new(StringComparer.OrdinalIgnoreCase);
    private readonly Dictionary<string, Quest> _knownQuests = new(StringComparer.OrdinalIgnoreCase);

    public bool SyncMapsFromAssets()
    {
        try
        {
            var assetsMaps = EngineServices.AssetsManager.GetAssetsOfType<MapDefinition>().ToList();
            _knownMaps.Clear();

            foreach (var mapDef in assetsMaps)
            {
                var width = Math.Max(1, (int)MathF.Round(mapDef.Size.Width));
                var height = Math.Max(1, (int)MathF.Round(mapDef.Size.Height));
                var map = new Map(mapDef.Unique.ToString(), mapDef.Name, width, height);

                foreach (var layer in mapDef.TileLayers)
                {
                    map.Layers.Add(new TileLayer(width, height, layer.Name));
                }

                if (map.Layers.Count == 0)
                {
                    map.Layers.Add(new TileLayer(width, height, "Base Layer"));
                }

                _knownMaps[map.Id] = map;
                _mapManager.Register(map);
            }

            _logger.Info("RPG systems map sync completed. Found {Count} maps.", args: assetsMaps.Count);
            return true;
        }
        catch (Exception ex)
        {
            _logger.Error(ex, "Failed to sync maps from assets.");
            return false;
        }
    }

    public bool TrySetCurrentMap(string mapId)
    {
        return _mapManager.TrySwitchTo(mapId);
    }

    public bool IsTilePassable(int x, int y)
    {
        return _mapManager.CurrentMap?.IsPassable(x, y) ?? false;
    }

    public void SetSwitch(string name, bool value)
    {
        _eventManager.Switches[name] = value;
    }

    public bool GetSwitch(string name, bool defaultValue = false)
    {
        return _eventManager.Switches.TryGetValue(name, out var value) ? value : defaultValue;
    }

    public void SetVariable(string name, int value)
    {
        _eventManager.Variables[name] = value;
    }

    public int GetVariable(string name, int defaultValue = 0)
    {
        return _eventManager.Variables.TryGetValue(name, out var value) ? value : defaultValue;
    }

    public bool RegisterSimpleSwitchEvent(string mapId, string eventId, int x, int y, RpgEventTriggerType triggerType, string switchName, bool switchValue)
    {
        if (!_knownMaps.TryGetValue(mapId, out var map))
        {
            return false;
        }

        var gameEvent = new GameEvent
        {
            Id = eventId,
            X = x,
            Y = y
        };

        gameEvent.Pages.Add(new EventPage
        {
            TriggerType = ToCoreTrigger(triggerType),
            Command = context => context.Switches[switchName] = switchValue
        });

        map.Events.Add(gameEvent);
        return true;
    }

    public bool TriggerEvent(string eventId, RpgEventTriggerType triggerType)
    {
        var map = _mapManager.CurrentMap;
        if (map is null)
        {
            return false;
        }

        var gameEvent = map.Events.FirstOrDefault(evt => string.Equals(evt.Id, eventId, StringComparison.OrdinalIgnoreCase));
        if (gameEvent is null)
        {
            return false;
        }

        _eventManager.Trigger(gameEvent, _party, ToCoreTrigger(triggerType));
        return true;
    }

    public void RegisterQuest(string questId, string questName)
    {
        var quest = new Quest
        {
            Id = questId,
            Name = questName
        };

        _knownQuests[questId] = quest;
        _questManager.Register(quest);
    }

    public bool AddQuestStep(string questId, string stepDescription)
    {
        if (!_knownQuests.TryGetValue(questId, out var quest))
        {
            return false;
        }

        quest.Steps.Add(new QuestStep { Description = stepDescription });
        return true;
    }

    public bool StartQuest(string questId)
    {
        return _questManager.Start(questId);
    }

    public bool CompleteQuestStep(string questId, int stepIndex)
    {
        if (!_knownQuests.TryGetValue(questId, out var quest))
        {
            return false;
        }

        if (stepIndex < 0 || stepIndex >= quest.Steps.Count)
        {
            return false;
        }

        quest.Steps[stepIndex].IsCompleted = true;
        _questManager.RefreshCompletion(questId);
        return true;
    }

    public RpgQuestStatus GetQuestStatus(string questId)
    {
        if (!_knownQuests.TryGetValue(questId, out var quest))
        {
            return RpgQuestStatus.Unknown;
        }

        return quest.Status switch
        {
            QuestStatus.Inactive => RpgQuestStatus.Inactive,
            QuestStatus.Active => RpgQuestStatus.Active,
            QuestStatus.Completed => RpgQuestStatus.Completed,
            QuestStatus.Failed => RpgQuestStatus.Failed,
            _ => RpgQuestStatus.Unknown
        };
    }

    public bool SaveRuntime(string path)
    {
        var mapId = _mapManager.CurrentMap?.Id;
        if (string.IsNullOrWhiteSpace(mapId))
        {
            return false;
        }

        var data = new SaveData
        {
            CurrentMapId = mapId,
            PartyActorIds = _party.Members.Select(actor => actor.Id).ToList(),
            Switches = new Dictionary<string, bool>(_eventManager.Switches, StringComparer.OrdinalIgnoreCase),
            Variables = new Dictionary<string, int>(_eventManager.Variables, StringComparer.OrdinalIgnoreCase)
        };

        _saveManager.Save(path, data);
        return true;
    }

    public bool LoadRuntime(string path)
    {
        var data = _saveManager.Load(path);
        if (data is null)
        {
            return false;
        }

        _eventManager.Switches.Clear();
        foreach (var pair in data.Switches)
        {
            _eventManager.Switches[pair.Key] = pair.Value;
        }

        _eventManager.Variables.Clear();
        foreach (var pair in data.Variables)
        {
            _eventManager.Variables[pair.Key] = pair.Value;
        }

        return TrySetCurrentMap(data.CurrentMapId);
    }

    private static EventTriggerType ToCoreTrigger(RpgEventTriggerType triggerType)
    {
        return triggerType switch
        {
            RpgEventTriggerType.Action => EventTriggerType.Action,
            RpgEventTriggerType.Autorun => EventTriggerType.Autorun,
            RpgEventTriggerType.Parallel => EventTriggerType.Parallel,
            RpgEventTriggerType.PlayerTouch => EventTriggerType.PlayerTouch,
            _ => EventTriggerType.Action
        };
    }
}
