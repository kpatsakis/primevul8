TiledInputFile::frameBuffer () const
{
    Lock lock (*_data->_streamData);
    return _data->frameBuffer;
}