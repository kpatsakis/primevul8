    void endWriteStruct(const FStructData& current_struct)
    {
        int struct_flags = current_struct.flags;
        CV_Assert( FileNode::isCollection(struct_flags) );

        if( !FileNode::isFlow(struct_flags) )
        {
#if 0
            if ( fs->bufferPtr() <= fs->bufferStart() + fs->space )
            {
                /* some bad code for base64_writer... */
                ptr = fs->bufferPtr();
                *ptr++ = '\n';
                *ptr++ = '\0';
                fs->puts( fs->bufferStart() );
                fs->setBufferPtr(fs->bufferStart());
            }
#endif
            fs->flush();
        }

        char* ptr = fs->bufferPtr();
        if( ptr > fs->bufferStart() + current_struct.indent && !FileNode::isEmptyCollection(struct_flags) )
            *ptr++ = ' ';
        *ptr++ = FileNode::isMap(struct_flags) ? '}' : ']';
        fs->setBufferPtr(ptr);
    }