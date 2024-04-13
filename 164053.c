void RemoteFsDevice::Details::load(const QString &group)
{
    Configuration cfg(constCfgPrefix+group);
    name=group;
    QString u=cfg.get("url", QString());

    if (u.isEmpty()) {
        QString protocol=cfg.get("protocol", QString());
        QString host=cfg.get("host", QString());
        QString user=cfg.get("user", QString());
        QString path=cfg.get("path", QString());
        int port=cfg.get("port", 0);
        u=(protocol.isEmpty() ? "file" : protocol+"://")+user+(!user.isEmpty() && !host.isEmpty() ? "@" : "")+host+(0==port ? "" : (":"+QString::number(port)))+path;
    }
    if (!u.isEmpty()) {
        url=QUrl(u);
    }
    extraOptions=cfg.get("extraOptions", QString());
    configured=cfg.get("configured", configured);
}