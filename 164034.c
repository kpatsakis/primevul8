void RemoteFsDevice::renamed(const QString &oldName, const QString &newName)
{
    Configuration cfg;
    QStringList names=cfg.get(constCfgKey, QStringList());
    if (names.contains(oldName)) {
        names.removeAll(oldName);
        cfg.removeGroup(createUdi(oldName));
    }
    if (!names.contains(newName)) {
        names.append(newName);
    }
    cfg.set(constCfgKey, names);
}