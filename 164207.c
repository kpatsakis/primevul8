    void Greeter::onReadyReadStandardOutput()
    {
        if (m_process) {
            qDebug() << "Greeter output:" << qPrintable(QString::fromLocal8Bit(m_process->readAllStandardOutput()));
        }
    }