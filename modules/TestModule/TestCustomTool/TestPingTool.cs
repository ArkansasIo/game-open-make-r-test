using System.Collections.ObjectModel;
using System.Numerics;
using RPGCreator.SDK;
using RPGCreator.SDK.GlobalState;
using RPGCreator.SDK.Inputs;
using RPGCreator.SDK.Logging;
using RPGCreator.SDK.Types;

namespace TestModule.TestCustomTool;

public class TestPingTool : ToolLogic
{
    public override URN ToolUrn { get; protected set; } = "rpgc".ToUrnNamespace().ToUrnModule("tools").ToUrn("test_ping_tool");
    public override string DisplayName => "Test Ping Tool";
    public override string Description => "Minimal tool used to validate module-side tool registration and toolbar integration.";
    public override string Icon => "mdi-radar";
    public override PipedPath Category => "General|Testing".ToPipedPath();
    public override EPayloadType PayloadType => EPayloadType.AllTiles;

    public override ObservableCollection<IToolParameter> GetParameters()
    {
        return [];
    }

    public override void UseAt(Vector2? absolutePosition = null, MouseButton button = MouseButton.Left)
    {
        Logger.Info("TestPingTool used at {Position} with {Button}.", args: [absolutePosition?.ToString() ?? "null", button.ToString()]);
    }
}
