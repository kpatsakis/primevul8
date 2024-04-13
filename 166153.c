    char* skipSpaces( char* ptr )
    {
        bool is_eof = false;
        bool is_completed = false;

        while ( is_eof == false && is_completed == false )
        {
            if (!ptr)
                CV_PARSE_ERROR_CPP("Invalid input");
            switch ( *ptr )
            {
                /* comment */
                case '/' : {
                    ptr++;
                    if ( *ptr == '\0' )
                    {
                        ptr = fs->gets();
                        if( !ptr || !*ptr ) { is_eof = true; break; }
                    }

                    if ( *ptr == '/' )
                    {
                        while ( *ptr != '\n' && *ptr != '\r' )
                        {
                            if ( *ptr == '\0' )
                            {
                                ptr = fs->gets();
                                if( !ptr || !*ptr ) { is_eof = true; break; }
                            }
                            else
                            {
                                ptr++;
                            }
                        }
                    }
                    else if ( *ptr == '*' )
                    {
                        ptr++;
                        for (;;)
                        {
                            if ( *ptr == '\0' )
                            {
                                ptr = fs->gets();
                                if( !ptr || !*ptr ) { is_eof = true; break; }
                            }
                            else if ( *ptr == '*' )
                            {
                                ptr++;
                                if ( *ptr == '\0' )
                                {
                                    ptr = fs->gets();
                                    if( !ptr || !*ptr ) { is_eof = true; break; }
                                }
                                if ( *ptr == '/' )
                                {
                                    ptr++;
                                    break;
                                }
                            }
                            else
                            {
                                ptr++;
                            }
                        }
                    }
                    else
                    {
                        CV_PARSE_ERROR_CPP( "Not supported escape character" );
                    }
                } break;
                    /* whitespace */
                case '\t':
                case ' ' : {
                    ptr++;
                } break;
                    /* newline || end mark */
                case '\0':
                case '\n':
                case '\r': {
                    ptr = fs->gets();
                    if( !ptr || !*ptr ) { is_eof = true; break; }
                } break;
                    /* other character */
                default: {
                    if( !cv_isprint(*ptr) )
                        CV_PARSE_ERROR_CPP( "Invalid character in the stream" );
                    is_completed = true;
                } break;
            }
        }

        if ( is_eof || !is_completed )
        {
            ptr = fs->bufferStart();
            CV_Assert(ptr);
            *ptr = '\0';
            fs->setEof();
            if( !is_completed )
                CV_PARSE_ERROR_CPP( "Abort at parse time" );
        }

        return ptr;
    }