RemoteDevicePropertiesWidget::RemoteDevicePropertiesWidget(QWidget *parent)
    : QWidget(parent)
    , modified(false)
    , saveable(false)
{
    setupUi(this);
    if (qobject_cast<QTabWidget *>(parent)) {
        verticalLayout->setMargin(4);
    }
    type->addItem(tr("Secure Shell (sshfs)"), (int)Type_SshFs);
    type->addItem(tr("Locally Mounted Folder"), (int)Type_File);
}