MultiPartInputFile::Data::createTileOffsets(const Header& header)
{
    //
    // Get the dataWindow information
    //

    const Box2i &dataWindow = header.dataWindow();
    int minX = dataWindow.min.x;
    int maxX = dataWindow.max.x;
    int minY = dataWindow.min.y;
    int maxY = dataWindow.max.y;

    //
    // Precompute level and tile information
    //

    int* numXTiles = nullptr;
    int* numYTiles = nullptr;
    int numXLevels, numYLevels;
    TileDescription tileDesc = header.tileDescription();
    try
    {

        precalculateTileInfo (tileDesc,
                            minX, maxX,
                            minY, maxY,
                            numXTiles, numYTiles,
                            numXLevels, numYLevels);

        TileOffsets* tileOffsets = new TileOffsets (tileDesc.mode,
                                                    numXLevels,
                                                    numYLevels,
                                                    numXTiles,
                                                    numYTiles);
        delete [] numXTiles;
        delete [] numYTiles;

        return tileOffsets;

    }
    catch(...)
    {
        delete [] numXTiles;
        delete [] numYTiles;
        throw;
    }

}