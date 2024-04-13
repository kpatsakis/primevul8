static inline bool isValid(const RemoteFsDevice::Details &d)
{
    return d.isLocalFile() || RemoteFsDevice::constSshfsProtocol==d.url.scheme();
}