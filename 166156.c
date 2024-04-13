    void write(const char* key, int value)
    {
        char buf[128], *ptr = fs::itoa( value, buf, 10 );
        writeScalar( key, ptr);
    }