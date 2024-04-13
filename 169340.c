    int ImageFactory::getType(const byte* data, long size)
    {
        MemIo memIo(data, size);
        return getType(memIo);
    }