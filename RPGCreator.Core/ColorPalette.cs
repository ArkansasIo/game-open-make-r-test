using System;
using System.Collections.Generic;
using System.Drawing;

namespace RPGCreator.Core
{
    public class ColorPalette
    {
        public string Name { get; set; }
        public Dictionary<string, Color> Colors { get; set; }

        public ColorPalette(string name)
        {
            Name = name;
            Colors = new Dictionary<string, Color>();
        }

        public void AddColor(string key, Color color)
        {
            Colors[key] = color;
        }

        public Color GetColor(string key)
        {
            return Colors.ContainsKey(key) ? Colors[key] : Color.Empty;
        }
    }

    public static class PaletteManager
    {
        private static readonly Dictionary<string, ColorPalette> Palettes = new();
        public static ColorPalette CurrentPalette { get; private set; }

        public static void RegisterPalette(ColorPalette palette)
        {
            Palettes[palette.Name] = palette;
            if (CurrentPalette == null)
                CurrentPalette = palette;
        }

        public static bool SwitchPalette(string name)
        {
            if (Palettes.ContainsKey(name))
            {
                CurrentPalette = Palettes[name];
                return true;
            }
            return false;
        }

        public static IEnumerable<string> GetPaletteNames() => Palettes.Keys;
    }
}
