#ifndef ENGINE_TILEMAP_H
#define ENGINE_TILEMAP_H

#include <QList>

struct MapTile
{
    int tileId;
    int regionId;
    bool blocked;
};

class TileMap
{
public:
    TileMap();
    TileMap(int width, int height, int layers);

    bool resize(int width, int height, int layers);

    int width() const { return mapWidth; }
    int height() const { return mapHeight; }
    int layers() const { return mapLayers; }

    bool setTile(int x, int y, int layer, int tileId);
    bool setBlocked(int x, int y, int layer, bool blocked);
    bool setRegionId(int x, int y, int layer, int regionId);

    int tileId(int x, int y, int layer) const;
    int regionId(int x, int y, int layer) const;

    bool isBlocked(int x, int y) const;
    bool isBlocked(int x, int y, int layer) const;

private:
    int indexOf(int x, int y, int layer) const;

    int mapWidth;
    int mapHeight;
    int mapLayers;
    QList<MapTile> tiles;
};

#endif
