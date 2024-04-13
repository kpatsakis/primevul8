static inline bool isMountable(const RemoteFsDevice::Details &d)
{
    return RemoteFsDevice::constSshfsProtocol==d.url.scheme();
}