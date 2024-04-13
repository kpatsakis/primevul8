Ptr<FileStorageEmitter> createYAMLEmitter(FileStorage_API* fs)
{
    return makePtr<YAMLEmitter>(fs);
}