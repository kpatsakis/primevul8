Device * RemoteFsDevice::create(MusicLibraryModel *m, const DeviceOptions &options, const Details &d)
{
    if (d.isEmpty()) {
        return 0;
    }
    Configuration cfg;
    QStringList names=cfg.get(constCfgKey, QStringList());
    if (names.contains(d.name)) {
        return 0;
    }
    names.append(d.name);
    cfg.set(constCfgKey, names);
    d.save();
    if (isValid(d)) {
        return new RemoteFsDevice(m, options, d);
    }
    return 0;
}