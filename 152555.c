TileBufferTask::TileBufferTask
    (TaskGroup *group,
     TiledInputFile::Data *ifd,
     TileBuffer *tileBuffer)
:
    Task (group),
    _ifd (ifd),
    _tileBuffer (tileBuffer)
{
    // empty
}