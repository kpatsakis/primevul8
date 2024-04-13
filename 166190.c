    void write(const char* key, const char* str, bool quote)
    {
        char buf[CV_FS_MAX_LEN*4+16];
        char* data = (char*)str;
        int i, len;

        if( !str )
            CV_Error( CV_StsNullPtr, "Null string pointer" );

        len = (int)strlen(str);
        if( len > CV_FS_MAX_LEN )
            CV_Error( CV_StsBadArg, "The written string is too long" );

        if( quote || len == 0 || str[0] != str[len-1] || (str[0] != '\"' && str[0] != '\'') )
        {
            int need_quote = 1;
            data = buf;
            *data++ = '\"';
            for( i = 0; i < len; i++ )
            {
                char c = str[i];

                switch ( c )
                {
                case '\\':
                case '\"':
                case '\'': { *data++ = '\\'; *data++ = c;   break; }
                case '\n': { *data++ = '\\'; *data++ = 'n'; break; }
                case '\r': { *data++ = '\\'; *data++ = 'r'; break; }
                case '\t': { *data++ = '\\'; *data++ = 't'; break; }
                case '\b': { *data++ = '\\'; *data++ = 'b'; break; }
                case '\f': { *data++ = '\\'; *data++ = 'f'; break; }
                default  : { *data++ = c; }
                }
            }

            *data++ = '\"';
            *data++ = '\0';
            data = buf + !need_quote;
        }

        writeScalar( key, data);
    }