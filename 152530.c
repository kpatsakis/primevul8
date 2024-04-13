TiledInputFile::isValidTile (int dx, int dy, int lx, int ly) const
{
    return ((lx < _data->numXLevels && lx >= 0) &&
            (ly < _data->numYLevels && ly >= 0) &&
            (dx < _data->numXTiles[lx] && dx >= 0) &&
            (dy < _data->numYTiles[ly] && dy >= 0));
}