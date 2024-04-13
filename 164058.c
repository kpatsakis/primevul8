void RemoteFsDevice::load()
{
    if (isConnected()) {
        setAudioFolder();
        readOpts(settingsFileName(), opts, true);
        rescan(false); // Read from cache if we have it!
    }
}