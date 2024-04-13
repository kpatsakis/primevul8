QString RemoteFsDevice::capacityString()
{
    if (cacheProgress>-1) {
        return statusMessage();
    }
    if (!isConnected()) {
        return tr("Not Connected");
    }

    spaceInfo.setPath(mountPoint(details, false));
    if (isOldSshfs()) {
        return tr("Capacity Unknown");
    }
    return tr("%1 free").arg(Utils::formatByteSize(spaceInfo.size()-spaceInfo.used()));
}