using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia.Controls;
using RPGCreator.SDK;
using RPGCreator.SDK.EditorUiService;
using RPGCreator.SDK.Modules.UIModule;
using RPGCreator.SDK.Types;
using RPGCreator.UI.Common.Modal.Browser;

namespace _BaseModule.UI.EditorExtensions;

internal static class BaseModuleEditorExtensions
{
    public static void Register(IUiExtensionManager extensionManager)
    {
        extensionManager.RegisterExtension(UIRegion.EditorMenuBar, (_, context) =>
        {
            if (context is not Menu menu)
            {
                return;
            }

            if (menu.Items is not IList<object> items)
            {
                return;
            }

            // Prevent duplicate menu registration when UI is recreated.
            if (items.OfType<MenuItem>().Any(i => string.Equals(i.Header?.ToString(), "Base Module", StringComparison.Ordinal)))
            {
                return;
            }

            var root = new MenuItem { Header = "Base Module" };
            var tools = new MenuItem { Header = "Tools" };

            var simplePen = new MenuItem { Header = "Activate Simple Pen" };
            simplePen.Click += (_, _) => ActivateTool(SimplePenUrn);

            var characterPlacer = new MenuItem { Header = "Activate Character Placer" };
            characterPlacer.Click += (_, _) => ActivateTool(CharacterPlacerUrn);

            var openBrowser = new MenuItem { Header = "Open Tools Browser" };
            openBrowser.Click += (_, _) =>
            {
                EditorUiServices.DialogService.ShowPromptAsync(
                    "Tools Browser",
                    new ToolsBrowser(),
                    new DialogStyle(Width: 900, Height: 500, SizeToContent: DialogSizeToContent.None));
            };

            tools.Items.Add(simplePen);
            tools.Items.Add(characterPlacer);
            tools.Items.Add(new Separator());
            tools.Items.Add(openBrowser);

            root.Items.Add(tools);
            items.Add(root);
        });

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

            // Adds a concrete tools entry in the assets manager.
            Func<UserControl> factory = () => new ToolsBrowser();
            registerMethod.Invoke(context, ["Tools", factory]);
        });
    }

    private static readonly URN SimplePenUrn = "rpgc".ToUrnNamespace().ToUrnModule("tools").ToUrn("simple_pen");
    private static readonly URN CharacterPlacerUrn = "rpgc".ToUrnNamespace().ToUrnModule("tools").ToUrn("character_placer");

    private static void ActivateTool(URN toolUrn)
    {
        var tool = RegistryServices.ToolRegistry.GetTool(toolUrn);
        if (tool != null)
        {
            RegistryServices.ToolRegistry.ActivateTool(tool);
        }
    }
}
