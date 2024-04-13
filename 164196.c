    void Greeter::authError(const QString &message, Auth::Error error) {
        Q_UNUSED(error);
        qWarning() << "Error from greeter session:" << message;
    }