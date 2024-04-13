static QString mountPoint(const RemoteFsDevice::Details &details, bool create)
{
    if (details.isLocalFile()) {
        return details.url.path();
    }
    return Utils::cacheDir(QLatin1String("mount/")+details.name, create);
}