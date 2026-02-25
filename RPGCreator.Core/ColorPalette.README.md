# Color Palette System

This project includes a flexible color palette system for consistent theming across the engine and UI.

## Features
- Define named color palettes (e.g., Light, Dark)
- Register and switch palettes at runtime
- Access colors by semantic keys (e.g., "Background", "Accent")
- Extensible for future themes or palette types

## Usage

### Palette Definition
Palettes are defined in `RPGCreator.Core/ColorPalette.cs`:

```csharp
var lightPalette = new ColorPalette("Light")
{
    Colors = new Dictionary<string, Color>
    {
        {"Background", Color.White},
        {"Foreground", Color.Black},
        {"Accent", Color.DeepSkyBlue},
        {"Highlight", Color.LightYellow}
    }
};
```

### Palette Registration & Switching
Palettes are registered and switched via `PaletteManager`:

```csharp
PaletteManager.RegisterPalette(lightPalette);
PaletteManager.SwitchPalette("Dark");
```

### Initialization
Palettes are initialized in `EngineCore.InitializeColorPalettes()` and called from the Desktop app entrypoint.

### Accessing Colors
Use `PaletteManager.CurrentPalette.GetColor("Background")` to retrieve colors for UI or rendering.

## Extending
- Add new palettes by creating `ColorPalette` instances and registering them.
- Add new color keys as needed for your UI.

## Integration
- Desktop app initializes palettes on startup.
- UnrealUI and other modules can be extended to use palette data for consistent theming.

---

For more details, see `RPGCreator.Core/ColorPalette.cs` and `EngineCore.cs`.
