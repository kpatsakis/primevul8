    void writeComment(const char* comment, bool eol_comment)
    {
        FStructData& current_struct = fs->getCurrentStruct();
        int len;
        int multiline;
        const char* eol;
        char* ptr;

        if( !comment )
            CV_Error( CV_StsNullPtr, "Null comment" );

        if( strstr(comment, "--") != 0 )
            CV_Error( CV_StsBadArg, "Double hyphen \'--\' is not allowed in the comments" );

        len = (int)strlen(comment);
        eol = strchr(comment, '\n');
        multiline = eol != 0;
        ptr = fs->bufferPtr();

        if( multiline || !eol_comment || fs->bufferEnd() - ptr < len + 5 )
            ptr = fs->flush();
        else if( ptr > fs->bufferStart() + current_struct.indent )
            *ptr++ = ' ';

        if( !multiline )
        {
            ptr = fs->resizeWriteBuffer( ptr, len + 9 );
            sprintf( ptr, "<!-- %s -->", comment );
            len = (int)strlen(ptr);
        }
        else
        {
            strcpy( ptr, "<!--" );
            len = 4;
        }

        fs->setBufferPtr(ptr + len);
        ptr = fs->flush();

        if( multiline )
        {
            while( comment )
            {
                if( eol )
                {
                    ptr = fs->resizeWriteBuffer( ptr, (int)(eol - comment) + 1 );
                    memcpy( ptr, comment, eol - comment + 1 );
                    ptr += eol - comment;
                    comment = eol + 1;
                    eol = strchr( comment, '\n' );
                }
                else
                {
                    len = (int)strlen(comment);
                    ptr = fs->resizeWriteBuffer( ptr, len );
                    memcpy( ptr, comment, len );
                    ptr += len;
                    comment = 0;
                }
                fs->setBufferPtr(ptr);
                ptr = fs->flush();
            }
            sprintf( ptr, "-->" );
            fs->setBufferPtr(ptr + 3);
            fs->flush();
        }
    }