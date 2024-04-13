Ptr<FileStorageParser> createXMLParser(FileStorage_API* fs)
{
    return makePtr<XMLParser>(fs);
}