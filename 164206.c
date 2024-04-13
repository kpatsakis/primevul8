    void Greeter::onSessionStarted(bool success) {
        // set flag
        m_started = success;

        // log message
        if (success)
            qDebug() << "Greeter session started successfully";
        else
            qDebug() << "Greeter session failed to start";
    }