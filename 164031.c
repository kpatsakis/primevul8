void RemoteFsDevice::saveProperties(const DeviceOptions &newOpts, const Details &nd)
{
    bool connected=isConnected();
    if (configured && (!connected || opts==newOpts) && (connected || details==nd)) {
        return;
    }

    bool isLocal=details.isLocalFile();

    if (connected) {
        if (!configured) {
            details.configured=configured=true;
            details.save();
        }
        if (opts.useCache!=newOpts.useCache) {
            if (opts.useCache) {
                saveCache();
            } else if (opts.useCache && !newOpts.useCache) {
                removeCache();
            }
        }
        opts=newOpts;
        writeOpts(settingsFileName(), opts, true);
    }
    if (!connected || isLocal) {
        Details newDetails=nd;
        Details oldDetails=details;
        bool newName=!oldDetails.name.isEmpty() && oldDetails.name!=newDetails.name;
        bool newDir=oldDetails.url.path()!=newDetails.url.path();

        if (isLocal && newDir && opts.useCache) {
            removeCache();
        }
        details=newDetails;
        details.configured=configured=true;
        details.save();

        if (newName) {
            if (!details.isLocalFile()) {
                QString oldMount=mountPoint(oldDetails, false);
                if (!oldMount.isEmpty() && QDir(oldMount).exists()) {
                    ::rmdir(QFile::encodeName(oldMount).constData());
                }
            }
            setData(details.name);
            renamed(oldDetails.name, details.name);
            deviceId=createUdi(details.name);
            emit udiChanged();
            m_itemData=details.name;
            setStatusMessage(QString());
        }
        if (isLocal && newDir && scanned) {
            rescan(true);
        }
    }
    emit configurationChanged();
}