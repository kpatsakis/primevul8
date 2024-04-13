    int ImageFactory::getType(const std::wstring& wpath)
    {
        FileIo fileIo(wpath);
        return getType(fileIo);
    }