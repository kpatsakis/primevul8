    void Greeter::onReadyReadStandardError()
    {
        if (m_process) {
            qDebug() << "Greeter errors:" << qPrintable(QString::fromLocal8Bit(m_process->readAllStandardError()));
        }
    }