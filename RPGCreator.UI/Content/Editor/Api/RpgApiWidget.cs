using System;
using System.IO;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Primitives;
using Avalonia.Layout;
using Avalonia.Media;
using RPGCreator.SDK;
using RPGCreator.SDK.EngineService;

namespace RPGCreator.UI.Content.Editor.Api;

public sealed class RpgApiWidget : UserControl
{
    private readonly ComboBox _mapCombo;
    private readonly TextBox _tileXBox;
    private readonly TextBox _tileYBox;
    private readonly TextBox _switchNameBox;
    private readonly TextBox _switchValueBox;
    private readonly TextBox _variableNameBox;
    private readonly TextBox _variableValueBox;
    private readonly TextBox _eventIdBox;
    private readonly ComboBox _eventTriggerCombo;
    private readonly TextBox _eventSwitchNameBox;
    private readonly TextBox _eventSwitchValueBox;
    private readonly TextBox _questIdBox;
    private readonly TextBox _questNameBox;
    private readonly TextBox _questStepBox;
    private readonly TextBox _questStepIndexBox;
    private readonly TextBox _savePathBox;
    private readonly TextBox _logBox;

    public RpgApiWidget()
    {
        var body = new Grid
        {
            Margin = new Thickness(10),
            RowDefinitions = new RowDefinitions("*, 220")
        };

        var sections = new StackPanel
        {
            Orientation = Orientation.Vertical,
            Spacing = 8
        };

        var scroll = new ScrollViewer
        {
            VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
            HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled,
            Content = sections
        };
        body.Children.Add(scroll);
        Grid.SetRow(scroll, 0);

        _logBox = new TextBox
        {
            IsReadOnly = true,
            AcceptsReturn = true,
            TextWrapping = TextWrapping.Wrap,
            VerticalContentAlignment = VerticalAlignment.Top
        };
        body.Children.Add(_logBox);
        Grid.SetRow(_logBox, 1);

        _mapCombo = new ComboBox { MinWidth = 260 };
        _tileXBox = new TextBox { Width = 80, Text = "0" };
        _tileYBox = new TextBox { Width = 80, Text = "0" };

        _switchNameBox = new TextBox { Width = 180, Watermark = "switch name", Text = "door_open" };
        _switchValueBox = new TextBox { Width = 80, Text = "true" };
        _variableNameBox = new TextBox { Width = 180, Watermark = "variable name", Text = "score" };
        _variableValueBox = new TextBox { Width = 80, Text = "10" };

        _eventIdBox = new TextBox { Width = 180, Text = "evt_001" };
        _eventTriggerCombo = new ComboBox { Width = 140, ItemsSource = Enum.GetValues<RpgEventTriggerType>(), SelectedItem = RpgEventTriggerType.Action };
        _eventSwitchNameBox = new TextBox { Width = 140, Text = "quest_started" };
        _eventSwitchValueBox = new TextBox { Width = 80, Text = "true" };

        _questIdBox = new TextBox { Width = 160, Text = "quest_main_001" };
        _questNameBox = new TextBox { Width = 200, Text = "Main Quest" };
        _questStepBox = new TextBox { Width = 280, Text = "Talk to the first NPC." };
        _questStepIndexBox = new TextBox { Width = 80, Text = "0" };

        _savePathBox = new TextBox
        {
            Width = 480,
            Text = Path.Combine(AppContext.BaseDirectory, "rpg_runtime_save.json")
        };

        sections.Children.Add(CreateMapSection());
        sections.Children.Add(CreateSwitchVariableSection());
        sections.Children.Add(CreateEventsSection());
        sections.Children.Add(CreateQuestSection());
        sections.Children.Add(CreateSaveLoadSection());

        Content = body;
        RefreshMaps();
        Log("RPG API widget ready.");
    }

    private Control CreateMapSection()
    {
        var panel = NewSection("Map / Tile");
        var row1 = NewRow();
        var row2 = NewRow();

        var syncBtn = NewButton("Sync Maps", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.SyncMapsFromAssets();
            RefreshMaps();
            Log($"SyncMapsFromAssets: {ok}");
        });
        var setCurrentBtn = NewButton("Set Current Map", (_, _) =>
        {
            if (_mapCombo.SelectedItem is not string mapId)
            {
                Log("No map selected.");
                return;
            }

            Log($"TrySetCurrentMap({mapId}): {EngineServices.RpgSystems.TrySetCurrentMap(mapId)}");
        });
        var passableBtn = NewButton("Check Passable", (_, _) =>
        {
            var x = ParseInt(_tileXBox.Text);
            var y = ParseInt(_tileYBox.Text);
            Log($"IsTilePassable({x}, {y}): {EngineServices.RpgSystems.IsTilePassable(x, y)}");
        });

        row1.Children.Add(syncBtn);
        row1.Children.Add(new TextBlock { Text = "Map:", VerticalAlignment = VerticalAlignment.Center });
        row1.Children.Add(_mapCombo);
        row1.Children.Add(setCurrentBtn);

        row2.Children.Add(new TextBlock { Text = "X:", VerticalAlignment = VerticalAlignment.Center });
        row2.Children.Add(_tileXBox);
        row2.Children.Add(new TextBlock { Text = "Y:", VerticalAlignment = VerticalAlignment.Center });
        row2.Children.Add(_tileYBox);
        row2.Children.Add(passableBtn);

        panel.Children.Add(row1);
        panel.Children.Add(row2);
        return panel;
    }

    private Control CreateSwitchVariableSection()
    {
        var panel = NewSection("Switch / Variable");
        var row1 = NewRow();
        var row2 = NewRow();

        row1.Children.Add(_switchNameBox);
        row1.Children.Add(_switchValueBox);
        row1.Children.Add(NewButton("Set Switch", (_, _) =>
        {
            var value = ParseBool(_switchValueBox.Text);
            EngineServices.RpgSystems.SetSwitch(_switchNameBox.Text ?? string.Empty, value);
            Log($"SetSwitch({_switchNameBox.Text}, {value})");
        }));
        row1.Children.Add(NewButton("Get Switch", (_, _) =>
        {
            var value = EngineServices.RpgSystems.GetSwitch(_switchNameBox.Text ?? string.Empty);
            Log($"GetSwitch({_switchNameBox.Text}) = {value}");
        }));

        row2.Children.Add(_variableNameBox);
        row2.Children.Add(_variableValueBox);
        row2.Children.Add(NewButton("Set Variable", (_, _) =>
        {
            var value = ParseInt(_variableValueBox.Text);
            EngineServices.RpgSystems.SetVariable(_variableNameBox.Text ?? string.Empty, value);
            Log($"SetVariable({_variableNameBox.Text}, {value})");
        }));
        row2.Children.Add(NewButton("Get Variable", (_, _) =>
        {
            var value = EngineServices.RpgSystems.GetVariable(_variableNameBox.Text ?? string.Empty);
            Log($"GetVariable({_variableNameBox.Text}) = {value}");
        }));

        panel.Children.Add(row1);
        panel.Children.Add(row2);
        return panel;
    }

    private Control CreateEventsSection()
    {
        var panel = NewSection("Events");
        var row1 = NewRow();
        var row2 = NewRow();

        row1.Children.Add(new TextBlock { Text = "Event ID:", VerticalAlignment = VerticalAlignment.Center });
        row1.Children.Add(_eventIdBox);
        row1.Children.Add(new TextBlock { Text = "Trigger:", VerticalAlignment = VerticalAlignment.Center });
        row1.Children.Add(_eventTriggerCombo);
        row1.Children.Add(NewButton("Trigger Event", (_, _) =>
        {
            var trigger = (RpgEventTriggerType)(_eventTriggerCombo.SelectedItem ?? RpgEventTriggerType.Action);
            var ok = EngineServices.RpgSystems.TriggerEvent(_eventIdBox.Text ?? string.Empty, trigger);
            Log($"TriggerEvent({_eventIdBox.Text}, {trigger}) = {ok}");
        }));

        row2.Children.Add(new TextBlock { Text = "Switch:", VerticalAlignment = VerticalAlignment.Center });
        row2.Children.Add(_eventSwitchNameBox);
        row2.Children.Add(new TextBlock { Text = "Value:", VerticalAlignment = VerticalAlignment.Center });
        row2.Children.Add(_eventSwitchValueBox);
        row2.Children.Add(NewButton("Register Simple Event", (_, _) =>
        {
            if (_mapCombo.SelectedItem is not string mapId)
            {
                Log("No map selected for event registration.");
                return;
            }

            var trigger = (RpgEventTriggerType)(_eventTriggerCombo.SelectedItem ?? RpgEventTriggerType.Action);
            var ok = EngineServices.RpgSystems.RegisterSimpleSwitchEvent(
                mapId,
                _eventIdBox.Text ?? string.Empty,
                ParseInt(_tileXBox.Text),
                ParseInt(_tileYBox.Text),
                trigger,
                _eventSwitchNameBox.Text ?? string.Empty,
                ParseBool(_eventSwitchValueBox.Text));
            Log($"RegisterSimpleSwitchEvent({_eventIdBox.Text}) = {ok}");
        }));

        panel.Children.Add(row1);
        panel.Children.Add(row2);
        return panel;
    }

    private Control CreateQuestSection()
    {
        var panel = NewSection("Quest");
        var row1 = NewRow();
        var row2 = NewRow();

        row1.Children.Add(_questIdBox);
        row1.Children.Add(_questNameBox);
        row1.Children.Add(NewButton("Register Quest", (_, _) =>
        {
            EngineServices.RpgSystems.RegisterQuest(_questIdBox.Text ?? string.Empty, _questNameBox.Text ?? string.Empty);
            Log($"RegisterQuest({_questIdBox.Text})");
        }));
        row1.Children.Add(NewButton("Start Quest", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.StartQuest(_questIdBox.Text ?? string.Empty);
            Log($"StartQuest({_questIdBox.Text}) = {ok}");
        }));
        row1.Children.Add(NewButton("Status", (_, _) =>
        {
            var status = EngineServices.RpgSystems.GetQuestStatus(_questIdBox.Text ?? string.Empty);
            Log($"GetQuestStatus({_questIdBox.Text}) = {status}");
        }));

        row2.Children.Add(_questStepBox);
        row2.Children.Add(NewButton("Add Step", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.AddQuestStep(_questIdBox.Text ?? string.Empty, _questStepBox.Text ?? string.Empty);
            Log($"AddQuestStep({_questIdBox.Text}) = {ok}");
        }));
        row2.Children.Add(new TextBlock { Text = "Step#", VerticalAlignment = VerticalAlignment.Center });
        row2.Children.Add(_questStepIndexBox);
        row2.Children.Add(NewButton("Complete Step", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.CompleteQuestStep(_questIdBox.Text ?? string.Empty, ParseInt(_questStepIndexBox.Text));
            Log($"CompleteQuestStep({_questIdBox.Text}) = {ok}");
        }));

        panel.Children.Add(row1);
        panel.Children.Add(row2);
        return panel;
    }

    private Control CreateSaveLoadSection()
    {
        var panel = NewSection("Save / Load Runtime");
        var row = NewRow();

        row.Children.Add(_savePathBox);
        row.Children.Add(NewButton("Save Runtime", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.SaveRuntime(_savePathBox.Text ?? string.Empty);
            Log($"SaveRuntime({_savePathBox.Text}) = {ok}");
        }));
        row.Children.Add(NewButton("Load Runtime", (_, _) =>
        {
            var ok = EngineServices.RpgSystems.LoadRuntime(_savePathBox.Text ?? string.Empty);
            Log($"LoadRuntime({_savePathBox.Text}) = {ok}");
        }));

        panel.Children.Add(row);
        return panel;
    }

    private static StackPanel NewSection(string title)
    {
        var section = new StackPanel
        {
            Spacing = 6,
            Margin = new Thickness(0, 2, 0, 6)
        };

        section.Children.Add(new TextBlock
        {
            Text = title,
            FontWeight = Avalonia.Media.FontWeight.Bold
        });
        return section;
    }

    private static StackPanel NewRow()
    {
        return new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 8,
            VerticalAlignment = VerticalAlignment.Center
        };
    }

    private static Button NewButton(string text, EventHandler<Avalonia.Interactivity.RoutedEventArgs> handler)
    {
        var button = new Button
        {
            Content = text,
            MinWidth = 120
        };
        button.Click += handler;
        return button;
    }

    private void RefreshMaps()
    {
        _mapCombo.ItemsSource = null;
        _mapCombo.ItemsSource = EngineServices.RpgSystems.KnownMapIds;
        _mapCombo.SelectedItem = EngineServices.RpgSystems.CurrentMapId;
    }

    private void Log(string message)
    {
        _logBox.Text = $"{DateTime.Now:HH:mm:ss} - {message}{Environment.NewLine}{_logBox.Text}";
    }

    private static bool ParseBool(string? text)
    {
        return bool.TryParse(text, out var value) && value;
    }

    private static int ParseInt(string? text)
    {
        return int.TryParse(text, out var value) ? value : 0;
    }
}
