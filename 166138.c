Ptr<FileStorageParser> createJSONParser(FileStorage_API* fs)
{
    return makePtr<JSONParser>(fs);
}