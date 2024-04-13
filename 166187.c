    void endWriteStruct(const FStructData& current_struct)
    {
        char* ptr;

        int struct_flags = current_struct.flags;

        if( FileNode::isFlow(struct_flags) )
        {
            ptr = fs->bufferPtr();
            if( ptr > fs->bufferStart() + current_struct.indent && !FileNode::isEmptyCollection(struct_flags) )
                *ptr++ = ' ';
            *ptr++ = FileNode::isMap(struct_flags) ? '}' : ']';
            fs->setBufferPtr(ptr);
        }
        else if( FileNode::isEmptyCollection(struct_flags) )
        {
            ptr = fs->flush();
            memcpy( ptr, FileNode::isMap(struct_flags) ? "{}" : "[]", 2 );
            fs->setBufferPtr(ptr + 2);
        }
        /*
        if( !FileNode::isFlow(parent_flags) )
            fs->struct_indent -= CV_YML_INDENT + FileNode::isFlow(struct_flags);
        assert( fs->struct_indent >= 0 );*/
    }