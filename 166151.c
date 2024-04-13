    void writeScalar(const char* key, const char* data)
    {
        int len = (int)strlen(data);
        if( key && *key == '\0' )
            key = 0;

        FStructData& current_struct = fs->getCurrentStruct();
        int struct_flags = current_struct.flags;

        if( FileNode::isMap(struct_flags) ||
           (!FileNode::isCollection(struct_flags) && key) )
        {
            writeTag( key, CV_XML_OPENING_TAG );
            char* ptr = fs->resizeWriteBuffer( fs->bufferPtr(), len );
            memcpy( ptr, data, len );
            fs->setBufferPtr( ptr + len );
            writeTag( key, CV_XML_CLOSING_TAG );
        }
        else
        {
            char* ptr = fs->bufferPtr();
            int new_offset = (int)(ptr - fs->bufferStart()) + len;

            if( key )
                CV_Error( CV_StsBadArg, "elements with keys can not be written to sequence" );

            current_struct.flags = FileNode::SEQ;

            if( (new_offset > fs->wrapMargin() && new_offset - current_struct.indent > 10) ||
               (ptr > fs->bufferStart() && ptr[-1] == '>') )
            {
                ptr = fs->flush();
            }
            else if( ptr > fs->bufferStart() + current_struct.indent && ptr[-1] != '>' )
                *ptr++ = ' ';

            memcpy( ptr, data, len );
            fs->setBufferPtr(ptr + len);
        }
    }