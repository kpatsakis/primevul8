void RemoteFsDevice::umountStatus(const QString &mp, int pid, int st)
{
    if (pid==getpid() && mp==mountPoint(details, false)) {
        messageSent=false;
        if (0!=st) {
            emit error(tr("Failed to disconnect from \"%1\"").arg(details.name));
            setStatusMessage(QString());
        } else {
            setStatusMessage(QString());
            update=new MusicLibraryItemRoot;
            emit updating(id(), false);
            emit connectionStateHasChanged(id(), false);
        }
    }
}