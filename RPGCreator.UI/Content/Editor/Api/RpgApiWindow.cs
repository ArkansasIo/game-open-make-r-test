using Avalonia.Controls;

namespace RPGCreator.UI.Content.Editor.Api;

public sealed class RpgApiWindow : Window
{
    public RpgApiWindow()
    {
        Title = "RPG API Tools";
        Width = 980;
        Height = 760;
        MinWidth = 800;
        MinHeight = 600;
        WindowStartupLocation = WindowStartupLocation.CenterOwner;
        Content = new RpgApiWidget();
    }
}
