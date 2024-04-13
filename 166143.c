    void write( const char* key, double value )
    {
        char buf[128];
        writeScalar( key, fs::doubleToString( buf, value, false ) );
    }