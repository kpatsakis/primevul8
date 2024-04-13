qint64 RemoteFsDevice::freeSpace()
{
    if (!isConnected()) { // || !details.isLocalFile()) {
        return 0;
    }

    spaceInfo.setPath(mountPoint(details, false));
    if (isOldSshfs()) {
        return 0;
    }
    return spaceInfo.size()-spaceInfo.used();
}