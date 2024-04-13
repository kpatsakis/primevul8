TiledInputFile::~TiledInputFile ()
{
    if (!_data->memoryMapped)
        for (size_t i = 0; i < _data->tileBuffers.size(); i++)
            delete [] _data->tileBuffers[i]->buffer;

    if (_data->_deleteStream)
        delete _data->_streamData->is;

    if (_data->partNumber == -1)
        delete _data->_streamData;

    delete _data;
}