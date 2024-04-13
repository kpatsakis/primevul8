void RemoteFsDevice::procFinished(int exitCode)
{
    bool wasMount=proc->property("mount").isValid();
    proc->deleteLater();
    proc=0;

    if (0!=exitCode) {
        emit error(wasMount ? tr("Failed to connect to \"%1\"").arg(details.name)
                            : tr("Failed to disconnect from \"%1\"").arg(details.name));
        setStatusMessage(QString());
    } else if (wasMount) {
        setStatusMessage(tr("Updating tracks..."));
        load();
        emit connectionStateHasChanged(id(), true);
    } else {
        setStatusMessage(QString());
        update=new MusicLibraryItemRoot;
        scanned=false;
        emit updating(id(), false);
        emit connectionStateHasChanged(id(), false);
    }
}