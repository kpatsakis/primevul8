    void Greeter::authInfo(const QString &message, Auth::Info info) {
        Q_UNUSED(info);
        qDebug() << "Information from greeter session:" << message;
    }