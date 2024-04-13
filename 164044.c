void RemoteFsDevice::configure(QWidget *parent)
{
    if (isRefreshing()) {
        return;
    }

    RemoteDevicePropertiesDialog *dlg=new RemoteDevicePropertiesDialog(parent);
    connect(dlg, SIGNAL(updatedSettings(const DeviceOptions &, const RemoteFsDevice::Details &)),
            SLOT(saveProperties(const DeviceOptions &, const RemoteFsDevice::Details &)));
    if (!configured) {
        connect(dlg, SIGNAL(cancelled()), SLOT(saveProperties()));
    }
    dlg->show(opts, details,
              DevicePropertiesWidget::Prop_All-(DevicePropertiesWidget::Prop_Name+DevicePropertiesWidget::Prop_Folder+DevicePropertiesWidget::Prop_AutoScan),
              0, false, isConnected());
}