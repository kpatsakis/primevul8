Ptr<FileStorageParser> createYAMLParser(FileStorage_API* fs)
{
    return makePtr<YAMLParser>(fs);
}