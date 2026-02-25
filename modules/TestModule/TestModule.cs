using RPGCreator.SDK;
using RPGCreator.SDK.EditorUiService;
using RPGCreator.SDK.Logging;
using RPGCreator.SDK.Modules;
using RPGCreator.SDK.Types;
using TestModule.TestCustomFeature;
using TestModule.TestCustomTool;

[assembly: ModuleManifest(
    urn: "rpgc://module/test_module",
    name: "Test Module",
    Author = "Your Name",
    TargetEngineVersion = "1.0.0",
    Description = "A test module for RPG Creator.",
    Dependencies = [],
    Incompatibilities = []
)]

namespace TestModule;

public class TestModule : BaseModule
{
    protected override void OnInitialize()
    {
        Logger.Info("Test Module initialized.");

        EngineServices.FeaturesManager.RegisterEntityFeature<MyVeryOwnTestDependencyFeature>();
        RegistryServices.ToolRegistry.RegisterTool(new TestPingTool());

        EditorUiServices.OnceServiceReady((IUiExtensionManager extensionManager) =>
        {
            TestModuleUi.Register(extensionManager);
        });

        EngineServices.ModulePathResolver.RegisterPath(
            new URN("Ward727", "module", "TestModule/Folder"),
            Path.GetDirectoryName(typeof(TestModule).Assembly.Location));
    }

    protected override void OnShutdown()
    {
    }
}
