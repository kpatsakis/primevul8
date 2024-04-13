    void write(const char* key, int value)
    {
        char buf[128];
        writeScalar( key, fs::itoa( value, buf, 10 ));
    }