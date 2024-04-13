QString RemoteFsDevice::settingsFileName() const
{
    if (audioFolder.isEmpty()) {
        setAudioFolder();
    }
    return audioFolder+constCantataSettingsFile;
}