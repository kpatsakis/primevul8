void RemoteFsDevice::mountStatus(const QString &mp, int pid, int st)
{
    if (pid==getpid() && mp==mountPoint(details, false)) {
        messageSent=false;
        if (0!=st) {
            emit error(tr("Failed to connect to \"%1\"").arg(details.name));
            setStatusMessage(QString());
        } else {
            setStatusMessage(tr("Updating tracks..."));
            load();
            emit connectionStateHasChanged(id(), true);
        }
    }
}