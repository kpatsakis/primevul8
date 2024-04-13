void RemoteFsDevice::setAudioFolder() const
{
    audioFolder=Utils::fixPath(mountPoint(details, true));
}