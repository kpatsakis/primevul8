void RemoteFsDevice::destroy(bool removeFromConfig)
{
    if (removeFromConfig) {
        Configuration cfg;
        QStringList names=cfg.get(constCfgKey, QStringList());
        if (names.contains(details.name)) {
            names.removeAll(details.name);
            cfg.removeGroup(id());
            cfg.set(constCfgKey, names);
        }
    }
    stopScanner();
    if (isConnected()) {
        unmount();
    }
    if (isMountable(details)) {
        QString mp=mountPoint(details, false);
        if (!mp.isEmpty()) {
            QDir d(mp);
            if (d.exists()) {
                d.rmdir(mp);
            }
        }
    }
    deleteLater();
}