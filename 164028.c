bool RemoteFsDevice::isConnected() const
{
    if (details.isLocalFile()) {
       if (QDir(details.url.path()).exists()) {
           return true;
       }
       clear();
       return false;
    }

    if (mountToken==MountPoints::self()->currentToken()) {
        return currentMountStatus;
    }
    QString mp=mountPoint(details, false);
    if (mp.isEmpty()) {
        clear();
        return false;
    }

    if (opts.useCache && !audioFolder.isEmpty() && QFile::exists(cacheFileName())) {
        currentMountStatus=true;
        return true;
    }

    mountToken=MountPoints::self()->currentToken();
    currentMountStatus=MountPoints::self()->isMounted(mp);
    if (currentMountStatus) {
        return true;
    }
    clear();
    return false;
}