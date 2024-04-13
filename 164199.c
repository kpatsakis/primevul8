    void Greeter::stop() {
        // check flag
        if (!m_started)
            return;

        // log message
        qDebug() << "Greeter stopping...";

        if (daemonApp->testing()) {
            // terminate process
            m_process->terminate();

            // wait for finished
            if (!m_process->waitForFinished(5000))
                m_process->kill();
        }
    }