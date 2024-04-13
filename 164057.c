void RemoteDevicePropertiesWidget::update(const RemoteFsDevice::Details &d, bool create, bool isConnected)
{
    int t=d.isLocalFile() ? Type_File : Type_SshFs;
    setEnabled(d.isLocalFile() || !isConnected);
    infoLabel->setVisible(create);
    orig=d;
    name->setText(d.name);
    sshPort->setValue(22);

    connectionNote->setVisible(!d.isLocalFile() && isConnected);
    sshFolder->setText(QString());
    sshHost->setText(QString());
    sshUser->setText(QString());
    fileFolder->setText(QString());

    switch (t) {
    case Type_SshFs: {
        sshFolder->setText(d.url.path());
        if (0!=d.url.port()) {
            sshPort->setValue(d.url.port());
        }
        sshHost->setText(d.url.host());
        sshUser->setText(d.url.userName());
        sshExtra->setText(d.extraOptions);
        break;
    }
    case Type_File:
        fileFolder->setText(d.url.path());
        break;
    }

    name->setEnabled(d.isLocalFile() || !isConnected);

    connect(type, SIGNAL(currentIndexChanged(int)), this, SLOT(setType()));
    for (int i=1; i<type->count(); ++i) {
        if (type->itemData(i).toInt()==t) {
            type->setCurrentIndex(i);
            stackedWidget->setCurrentIndex(i);
            break;
        }
    }
    connect(name, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    connect(sshHost, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    connect(sshUser, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    connect(sshFolder, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    connect(sshPort, SIGNAL(valueChanged(int)), this, SLOT(checkSaveable()));
    connect(sshExtra, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    connect(fileFolder, SIGNAL(textChanged(const QString &)), this, SLOT(checkSaveable()));
    modified=false;
    setType();
    checkSaveable();
}