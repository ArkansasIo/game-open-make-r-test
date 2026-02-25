using Avalonia;
using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;

namespace TestModule.UI;

public class TestModuleAssetsPanel : UserControl
{
    public TestModuleAssetsPanel()
    {
        Content = new Border
        {
            Padding = new Thickness(16),
            Child = new StackPanel
            {
                Spacing = 8,
                Children =
                {
                    new TextBlock
                    {
                        Text = "Test Module",
                        FontSize = 20,
                        FontWeight = FontWeight.Bold
                    },
                    new TextBlock
                    {
                        Text = "This panel is provided by module source files (feature/tool/menu/submenu integration test).",
                        TextWrapping = TextWrapping.Wrap
                    }
                }
            }
        };
    }
}
