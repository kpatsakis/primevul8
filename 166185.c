    void writeScalar(const char* key, const char* data)
    {
        size_t key_len = 0u;
        if( key && *key == '\0' )
            key = 0;
        if ( key )
        {
            key_len = strlen(key);
            if ( key_len == 0u )
                CV_Error( CV_StsBadArg, "The key is an empty" );
            else if ( static_cast<int>(key_len) > CV_FS_MAX_LEN )
                CV_Error( CV_StsBadArg, "The key is too long" );
        }

        size_t data_len = 0u;
        if ( data )
            data_len = strlen(data);

        FStructData& current_struct = fs->getCurrentStruct();
        int struct_flags = current_struct.flags;
        if( FileNode::isCollection(struct_flags) )
        {
            if ( (FileNode::isMap(struct_flags) ^ (key != 0)) )
                CV_Error( CV_StsBadArg, "An attempt to add element without a key to a map, "
                         "or add element with key to sequence" );
        } else {
            fs->setNonEmpty();
            struct_flags = FileNode::EMPTY | (key ? FileNode::MAP : FileNode::SEQ);
        }

        // start to write
        char* ptr = 0;

        if( FileNode::isFlow(struct_flags) )
        {
            int new_offset;
            ptr = fs->bufferPtr();
            if( !FileNode::isEmptyCollection(struct_flags) )
                *ptr++ = ',';
            new_offset = static_cast<int>(ptr - fs->bufferStart() + key_len + data_len);
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
            if ( !FileNode::isEmptyCollection(struct_flags) )
            {
                ptr = fs->bufferPtr();
                *ptr++ = ',';
                *ptr++ = '\n';
                *ptr++ = '\0';
                fs->puts( fs->bufferStart() );
                fs->setBufferPtr(fs->bufferStart());
            }
            ptr = fs->flush();
        }

        if( key )
        {
            if( !cv_isalpha(key[0]) && key[0] != '_' )
                CV_Error( CV_StsBadArg, "Key must start with a letter or _" );

            ptr = fs->resizeWriteBuffer( ptr, static_cast<int>(key_len) );
            *ptr++ = '\"';

            for( size_t i = 0u; i < key_len; i++ )
            {
                char c = key[i];

                ptr[i] = c;
                if( !cv_isalnum(c) && c != '-' && c != '_' && c != ' ' )
                    CV_Error( CV_StsBadArg, "Key names may only contain alphanumeric characters [a-zA-Z0-9], '-', '_' and ' '" );
            }

            ptr += key_len;
            *ptr++ = '\"';
            *ptr++ = ':';
            *ptr++ = ' ';
        }

        if( data )
        {
            ptr = fs->resizeWriteBuffer( ptr, static_cast<int>(data_len) );
            memcpy( ptr, data, data_len );
            ptr += data_len;
        }

        fs->setBufferPtr(ptr);
        current_struct.flags &= ~FileNode::EMPTY;
    }