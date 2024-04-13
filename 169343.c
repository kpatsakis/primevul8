    int ImageFactory::getType(const std::string& path)
    {
        FileIo fileIo(path);
        return getType(fileIo);
    }