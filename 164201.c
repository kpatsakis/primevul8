    void Greeter::onRequestChanged() {
        m_auth->request()->setFinishAutomatically(true);
    }