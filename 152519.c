TiledInputFile::Data::~Data ()
{
    delete [] numXTiles;
    delete [] numYTiles;

    for (size_t i = 0; i < tileBuffers.size(); i++)
        delete tileBuffers[i];

    if (multiPartBackwardSupport)
        delete multiPartFile;
}