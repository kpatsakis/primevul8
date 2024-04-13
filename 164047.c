RemoteFsDevice::Details RemoteDevicePropertiesWidget::details()
{
    int t=type->itemData(type->currentIndex()).toInt();
    RemoteFsDevice::Details det;

    det.name=name->text().trimmed();
    switch (t) {
    case Type_SshFs: {
        det.url.setHost(sshHost->text().trimmed());
        det.url.setUserName(sshUser->text().trimmed());
        det.url.setPath(sshFolder->text().trimmed());
        det.url.setPort(sshPort->value());
        det.url.setScheme(RemoteFsDevice::constSshfsProtocol);
        det.extraOptions=sshExtra->text().trimmed();
        break;
    }
    case Type_File: {
        QString path=fileFolder->text().trimmed();
        if (path.isEmpty()) {
            path="/";
        }
        det.url.setPath(path);
        det.url.setScheme(RemoteFsDevice::constFileProtocol);
        break;
    }

    }
    return det;
}