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
            int need_quote = quote || len == 0 || str[0] == ' ';
            data = buf;
            *data++ = '\"';
            for( i = 0; i < len; i++ )
            {
                char c = str[i];

                if( !need_quote && !cv_isalnum(c) && c != '_' && c != ' ' && c != '-' &&
                   c != '(' && c != ')' && c != '/' && c != '+' && c != ';' )
                    need_quote = 1;

                if( !cv_isalnum(c) && (!cv_isprint(c) || c == '\\' || c == '\'' || c == '\"') )
                {
                    *data++ = '\\';
                    if( cv_isprint(c) )
                        *data++ = c;
                    else if( c == '\n' )
                        *data++ = 'n';
                    else if( c == '\r' )
                        *data++ = 'r';
                    else if( c == '\t' )
                        *data++ = 't';
                    else
                    {
                        sprintf( data, "x%02x", c );
                        data += 3;
                    }
                }
                else
                    *data++ = c;
            }
            if( !need_quote && (cv_isdigit(str[0]) ||
                                str[0] == '+' || str[0] == '-' || str[0] == '.' ))
                need_quote = 1;

            if( need_quote )
                *data++ = '\"';
            *data++ = '\0';
            data = buf + !need_quote;
        }

        writeScalar( key, data);
    }