    void writeScalar(const char* key, const char* data)
    {
        int i, keylen = 0;
        int datalen = 0;
        char* ptr;

        FStructData& current_struct = fs->getCurrentStruct();

        int struct_flags = current_struct.flags;

        if( key && key[0] == '\0' )
            key = 0;

        if( FileNode::isCollection(struct_flags) )
        {
            if( (FileNode::isMap(struct_flags) ^ (key != 0)) )
                CV_Error( CV_StsBadArg, "An attempt to add element without a key to a map, "
                         "or add element with key to sequence" );
        }
        else
        {
            fs->setNonEmpty();
            struct_flags = FileNode::EMPTY | (key ? FileNode::MAP : FileNode::SEQ);
        }

        if( key )
        {
            keylen = (int)strlen(key);
            if( keylen == 0 )
                CV_Error( CV_StsBadArg, "The key is an empty" );

            if( keylen > CV_FS_MAX_LEN )
                CV_Error( CV_StsBadArg, "The key is too long" );
        }

        if( data )
            datalen = (int)strlen(data);

        if( FileNode::isFlow(struct_flags) )
        {
            ptr = fs->bufferPtr();
            if( !FileNode::isEmptyCollection(struct_flags) )
                *ptr++ = ',';
            int new_offset = (int)(ptr - fs->bufferStart()) + keylen + datalen;
            if( new_offset > fs->wrapMargin() && new_offset - current_struct.indent > 10 )
            {
                fs->setBufferPtr(ptr);
                ptr = fs->flush();
            }
            else
                *ptr++ = ' ';
        }
        else
        {
            ptr = fs->flush();
            if( !FileNode::isMap(struct_flags) )
            {
                *ptr++ = '-';
                if( data )
                    *ptr++ = ' ';
            }
        }

        if( key )
        {
            if( !cv_isalpha(key[0]) && key[0] != '_' )
                CV_Error( CV_StsBadArg, "Key must start with a letter or _" );

            ptr = fs->resizeWriteBuffer( ptr, keylen );

            for( i = 0; i < keylen; i++ )
            {
                char c = key[i];

                ptr[i] = c;
                if( !cv_isalnum(c) && c != '-' && c != '_' && c != ' ' )
                    CV_Error( CV_StsBadArg, "Key names may only contain alphanumeric characters [a-zA-Z0-9], '-', '_' and ' '" );
            }

            ptr += keylen;
            *ptr++ = ':';
            if( !FileNode::isFlow(struct_flags) && data )
                *ptr++ = ' ';
        }

        if( data )
        {
            ptr = fs->resizeWriteBuffer( ptr, datalen );
            memcpy( ptr, data, datalen );
            ptr += datalen;
        }

        fs->setBufferPtr(ptr);
        current_struct.flags &= ~FileNode::EMPTY;
    }