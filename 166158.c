    void writeTag( const char* key, int tag_type, const std::vector<std::string>& attrlist=std::vector<std::string>() )
    {
        char* ptr = fs->bufferPtr();
        int i, len = 0;
        FStructData& current_struct = fs->getCurrentStruct();
        int struct_flags = current_struct.flags;

        if( key && key[0] == '\0' )
            key = 0;

        if( tag_type == CV_XML_OPENING_TAG || tag_type == CV_XML_EMPTY_TAG )
        {
            if( FileNode::isCollection(struct_flags) )
            {
                if( FileNode::isMap(struct_flags) ^ (key != 0) )
                    CV_Error( CV_StsBadArg, "An attempt to add element without a key to a map, "
                             "or add element with key to sequence" );
            }
            else
            {
                struct_flags = FileNode::EMPTY + (key ? FileNode::MAP : FileNode::SEQ);
                //fs->is_first = 0;
            }

            if( !FileNode::isEmptyCollection(struct_flags) )
                ptr = fs->flush();
        }

        if( !key )
            key = "_";
        else if( key[0] == '_' && key[1] == '\0' )
            CV_Error( CV_StsBadArg, "A single _ is a reserved tag name" );

        len = (int)strlen( key );
        *ptr++ = '<';
        if( tag_type == CV_XML_CLOSING_TAG )
        {
            if( !attrlist.empty() )
                CV_Error( CV_StsBadArg, "Closing tag should not include any attributes" );
            *ptr++ = '/';
        }

        if( !cv_isalpha(key[0]) && key[0] != '_' )
            CV_Error( CV_StsBadArg, "Key should start with a letter or _" );

        ptr = fs->resizeWriteBuffer( ptr, len );
        for( i = 0; i < len; i++ )
        {
            char c = key[i];
            if( !cv_isalnum(c) && c != '_' && c != '-' )
                CV_Error( CV_StsBadArg, "Key name may only contain alphanumeric characters [a-zA-Z0-9], '-' and '_'" );
            ptr[i] = c;
        }
        ptr += len;

        int nattr = (int)attrlist.size();
        CV_Assert( nattr % 2 == 0 );

        for( i = 0; i < nattr; i += 2 )
        {
            size_t len0 = attrlist[i].size();
            size_t len1 = attrlist[i+1].size();
            CV_Assert( len0 > 0 );

            ptr = fs->resizeWriteBuffer( ptr, (int)(len0 + len1 + 4) );
            *ptr++ = ' ';

            memcpy( ptr, attrlist[i].c_str(), len0 );
            ptr += len0;
            *ptr++ = '=';
            *ptr++ = '\"';
            if( len1 > 0 )
                memcpy( ptr, attrlist[i+1].c_str(), len1 );
            ptr += len1;
            *ptr++ = '\"';
        }

        if( tag_type == CV_XML_EMPTY_TAG )
            *ptr++ = '/';
        *ptr++ = '>';
        fs->setBufferPtr(ptr);
        current_struct.flags = struct_flags & ~FileNode::EMPTY;
    }