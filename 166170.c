    void write(const char* key, const char* str, bool quote)
    {
        char buf[CV_FS_MAX_LEN*6+16];
        char* data = (char*)str;
        int i, len;

        if( !str )
            CV_Error( CV_StsNullPtr, "Null string pointer" );

        len = (int)strlen(str);
        if( len > CV_FS_MAX_LEN )
            CV_Error( CV_StsBadArg, "The written string is too long" );

        if( quote || len == 0 || str[0] != '\"' || str[0] != str[len-1] )
        {
            bool need_quote = quote || len == 0;
            data = buf;
            *data++ = '\"';
            for( i = 0; i < len; i++ )
            {
                char c = str[i];

                if( (uchar)c >= 128 || c == ' ' )
                {
                    *data++ = c;
                    need_quote = true;
                }
                else if( !cv_isprint(c) || c == '<' || c == '>' || c == '&' || c == '\'' || c == '\"' )
                {
                    *data++ = '&';
                    if( c == '<' )
                    {
                        memcpy(data, "lt", 2);
                        data += 2;
                    }
                    else if( c == '>' )
                    {
                        memcpy(data, "gt", 2);
                        data += 2;
                    }
                    else if( c == '&' )
                    {
                        memcpy(data, "amp", 3);
                        data += 3;
                    }
                    else if( c == '\'' )
                    {
                        memcpy(data, "apos", 4);
                        data += 4;
                    }
                    else if( c == '\"' )
                    {
                        memcpy( data, "quot", 4);
                        data += 4;
                    }
                    else
                    {
                        sprintf( data, "#x%02x", (uchar)c );
                        data += 4;
                    }
                    *data++ = ';';
                    need_quote = 1;
                }
                else
                    *data++ = c;
            }
            if( !need_quote && (cv_isdigit(str[0]) ||
                                str[0] == '+' || str[0] == '-' || str[0] == '.' ))
                need_quote = true;

            if( need_quote )
                *data++ = '\"';
            len = (int)(data - buf) - !need_quote;
            *data++ = '\0';
            data = buf + (int)!need_quote;
        }

        writeScalar( key, data );
    }