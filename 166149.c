    char* skipSpaces( char* ptr, int min_indent, int max_comment_indent )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid input");

        for(;;)
        {
            while( *ptr == ' ' )
                ptr++;
            if( *ptr == '#' )
            {
                if( ptr - fs->bufferStart() > max_comment_indent )
                    return ptr;
                *ptr = '\0';
            }
            else if( cv_isprint(*ptr) )
            {
                if( ptr - fs->bufferStart() < min_indent )
                    CV_PARSE_ERROR_CPP( "Incorrect indentation" );
                break;
            }
            else if( *ptr == '\0' || *ptr == '\n' || *ptr == '\r' )
            {
                ptr = fs->gets();
                if( !ptr )
                {
                    // emulate end of stream
                    ptr = fs->bufferStart();
                    ptr[0] = ptr[1] = ptr[2] = '.';
                    ptr[3] = '\0';
                    fs->setEof();
                    break;
                }
                else
                {
                    int l = (int)strlen(ptr);
                    if( ptr[l-1] != '\n' && ptr[l-1] != '\r' && !fs->eof() )
                        CV_PARSE_ERROR_CPP( "Too long string or a last string w/o newline" );
                }
            }
            else
                CV_PARSE_ERROR_CPP( *ptr == '\t' ? "Tabs are prohibited in YAML!" : "Invalid character" );
        }

        return ptr;
    }