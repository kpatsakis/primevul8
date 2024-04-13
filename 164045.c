bool RemoteFsDevice::canPlaySongs() const
{
    return details.isLocalFile() || HttpServer::self()->isAlive();
}