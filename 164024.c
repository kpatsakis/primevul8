bool RemoteFsDevice::isOldSshfs()
{
    return constSshfsProtocol==details.url.scheme() && 0==spaceInfo.used() && 1073741824000==spaceInfo.size();
}