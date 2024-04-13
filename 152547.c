TiledInputFile::Data::getTileBuffer (int number)
{
    return tileBuffers[number % tileBuffers.size()];
}