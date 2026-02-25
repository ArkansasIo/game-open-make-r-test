using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia.Controls;
using RPGCreator.SDK;
using RPGCreator.SDK.EditorUiService;
using RPGCreator.SDK.Logging;
using RPGCreator.SDK.Modules.UIModule;
using RPGCreator.SDK.Types;
using RPGCreator.UI.Common.Modal.Browser;
using TestModule.TestCustomTool;
using TestModule.UI;

namespace TestModule;

internal static class TestModuleUi
{
    private static readonly URN PingToolUrn = "rpgc".ToUrnNamespace().ToUrnModule("tools").ToUrn("test_ping_tool");

    public static void Register(IUiExtensionManager extensionManager)
    {
        extensionManager.RegisterExtension(UIRegion.AssetsManagerMenu, (_, context) =>
        {
            if (context == null)
            {
                return;
            }

            var registerMethod = context.GetType().GetMethod("RegisterAssetsMenuOption");
            if (registerMethod == null)
            {
                return;
            }

            Func<UserControl> factory = () => new TestModuleAssetsPanel();
            registerMethod.Invoke(context, ["Test Module", factory]);
        });

        extensionManager.RegisterExtension(UIRegion.EditorMenuBar, (_, context) =>
        {
            if (context is not Menu menu || menu.Items is not IList<object> items)
            {
                return;
            }

            if (items.OfType<MenuItem>().Any(i => string.Equals(i.Header?.ToString(), "Test Module", StringComparison.Ordinal)))
            {
                return;
            }

            var root = new MenuItem { Header = "Test Module" };
            var tools = new MenuItem { Header = "Tools" };
            var activatePing = new MenuItem { Header = "Activate Ping Tool" };
            activatePing.Click += (_, _) =>
            {
                var tool = RegistryServices.ToolRegistry.GetTool(PingToolUrn);
                if (tool != null)
                {
                    RegistryServices.ToolRegistry.ActivateTool(tool);
                }
                else
                {
                    Logger.Warning("Ping tool was not found in ToolRegistry.");
                }
            };

            var openToolBrowser = new MenuItem { Header = "Open Tools Browser" };
            openToolBrowser.Click += (_, _) =>
            {
                EditorUiServices.DialogService.ShowPromptAsync(
                    "Tools Browser",
                    new ToolsBrowser(),
                    new DialogStyle(Width: 900, Height: 500, SizeToContent: DialogSizeToContent.None));
            };

            tools.Items.Add(activatePing);
            tools.Items.Add(openToolBrowser);
            root.Items.Add(tools);
            items.Add(root);
        });
    }
}
