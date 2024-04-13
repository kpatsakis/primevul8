TiledInputFile::compatibilityInitialize(OPENEXR_IMF_INTERNAL_NAMESPACE::IStream& is)
{
    is.seekg(0);
    //
    // Construct a MultiPartInputFile, initialize TiledInputFile
    // with the part 0 data.
    // (TODO) maybe change the third parameter of the constructor of MultiPartInputFile later.
    //
    _data->multiPartBackwardSupport = true;
    _data->multiPartFile = new MultiPartInputFile(is, _data->numThreads);
    InputPartData* part = _data->multiPartFile->getPart(0);

    multiPartInitialize(part);
}