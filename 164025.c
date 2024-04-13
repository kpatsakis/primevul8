double RemoteFsDevice::usedCapacity()
{
    if (cacheProgress>-1) {
        return (cacheProgress*1.0)/100.0;
    }
    if (!isConnected()) {
        return -1.0;
    }

    spaceInfo.setPath(mountPoint(details, false));
    if (isOldSshfs()) {
        return -1.0;
    }
    return spaceInfo.size()>0 ? (spaceInfo.used()*1.0)/(spaceInfo.size()*1.0) : -1.0;
}