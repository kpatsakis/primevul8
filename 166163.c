Ptr<FileStorageEmitter> createXMLEmitter(FileStorage_API* fs)
{
    return makePtr<XMLEmitter>(fs);
}