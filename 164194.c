    void Greeter::onHelperFinished(Auth::HelperExitStatus status) {
        // reset flag
        m_started = false;

        // log message
        qDebug() << "Greeter stopped.";

        // clean up
        m_auth->deleteLater();
        m_auth = nullptr;
    }