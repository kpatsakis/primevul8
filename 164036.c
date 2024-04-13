QList<Device *> RemoteFsDevice::loadAll(MusicLibraryModel *m)
{
    QList<Device *> devices;
    Configuration cfg;
    QStringList names=cfg.get(constCfgKey, QStringList());
    for (const QString &n: names) {
        Details d;
        d.load(n);
        if (d.isEmpty()) {
            cfg.removeGroup(constCfgPrefix+n);
        } else if (isValid(d)) {
            devices.append(new RemoteFsDevice(m, d));
        }
    }
    return devices;
}