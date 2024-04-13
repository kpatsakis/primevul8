TiledInputFile::multiPartInitialize(InputPartData* part)
{
    if (part->header.type() != TILEDIMAGE)
        throw IEX_NAMESPACE::ArgExc("Can't build a TiledInputFile from a type-mismatched part.");

    _data->_streamData = part->mutex;
    _data->header = part->header;
    _data->version = part->version;
    _data->partNumber = part->partNumber;
    _data->memoryMapped = _data->_streamData->is->isMemoryMapped();
    initialize();
    _data->tileOffsets.readFrom(part->chunkOffsets,_data->fileIsComplete);
    _data->_streamData->currentPosition = _data->_streamData->is->tellg();
}