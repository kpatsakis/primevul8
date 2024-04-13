void RemoteFsDevice::setup()
{
    details.load(details.name);
    configured=details.configured;
    if (isConnected()) {
        readOpts(settingsFileName(), opts, true);
    }
    load();
}