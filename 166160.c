Ptr<FileStorageEmitter> createJSONEmitter(FileStorage_API* fs)
{
    return makePtr<JSONEmitter>(fs);
}