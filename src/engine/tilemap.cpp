#include "tilemap.h"

TileMap::TileMap()
{
    mapWidth = 0;
    mapHeight = 0;
    mapLayers = 0;
}

TileMap::TileMap(int width, int height, int layers)
{
    mapWidth = 0;
    mapHeight = 0;
    mapLayers = 0;
    resize(width, height, layers);
}

bool TileMap::resize(int width, int height, int layers)
{
    if (width <= 0 || height <= 0 || layers <= 0)
        return false;

    mapWidth = width;
    mapHeight = height;
    mapLayers = layers;
    tiles.clear();

    int count = width * height * layers;
    int i;
    for (i = 0; i < count; ++i)
    {
        MapTile tile;
        tile.tileId = 0;
        tile.regionId = 0;
        tile.blocked = false;
        tiles.append(tile);
    }

    return true;
}

bool TileMap::setTile(int x, int y, int layer, int tileId)
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return false;
    tiles[index].tileId = tileId;
    return true;
}

bool TileMap::setBlocked(int x, int y, int layer, bool blocked)
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return false;
    tiles[index].blocked = blocked;
    return true;
}

bool TileMap::setRegionId(int x, int y, int layer, int regionId)
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return false;
    tiles[index].regionId = regionId;
    return true;
}

int TileMap::tileId(int x, int y, int layer) const
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return 0;
    return tiles[index].tileId;
}

int TileMap::regionId(int x, int y, int layer) const
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return 0;
    return tiles[index].regionId;
}

bool TileMap::isBlocked(int x, int y) const
{
    int layer;
    for (layer = 0; layer < mapLayers; ++layer)
    {
        if (isBlocked(x, y, layer))
            return true;
    }
    return false;
}

bool TileMap::isBlocked(int x, int y, int layer) const
{
    int index = indexOf(x, y, layer);
    if (index < 0)
        return true;
    return tiles[index].blocked;
}

int TileMap::indexOf(int x, int y, int layer) const
{
    if (x < 0 || y < 0 || layer < 0)
        return -1;
    if (x >= mapWidth || y >= mapHeight || layer >= mapLayers)
        return -1;
    return layer * (mapWidth * mapHeight) + (y * mapWidth) + x;
}
