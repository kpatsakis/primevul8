void TiledInputFile::tileOrder(int dx[], int dy[], int lx[], int ly[]) const
{
   return _data->tileOffsets.getTileOrder(dx,dy,lx,ly);
}