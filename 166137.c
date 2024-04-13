    void writeComment(const char* comment, bool eol_comment)
    {
        if( !comment )
            CV_Error( CV_StsNullPtr, "Null comment" );

        int len = static_cast<int>(strlen(comment));
        char* ptr = fs->bufferPtr();
        const char* eol = strchr(comment, '\n');
        bool multiline = eol != 0;

        if( !eol_comment || multiline || fs->bufferEnd() - ptr < len || ptr == fs->bufferStart() )
            ptr = fs->flush();
        else
            *ptr++ = ' ';

        while( comment )
        {
            *ptr++ = '/';
            *ptr++ = '/';
            *ptr++ = ' ';
            if( eol )
            {
                ptr = fs->resizeWriteBuffer( ptr, (int)(eol - comment) + 1 );
                memcpy( ptr, comment, eol - comment + 1 );
                fs->setBufferPtr(ptr + (eol - comment));
                comment = eol + 1;
                eol = strchr( comment, '\n' );
            }
            else
            {
                len = (int)strlen(comment);
                ptr = fs->resizeWriteBuffer( ptr, len );
                memcpy( ptr, comment, len );
                fs->setBufferPtr(ptr + len);
                comment = 0;
            }
            ptr = fs->flush();
        }
    }