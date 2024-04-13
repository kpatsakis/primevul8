    FStructData startWriteStruct(const FStructData& parent, const char* key,
                                 int struct_flags, const char* type_name=0)
    {
        char buf[CV_FS_MAX_LEN + 1024];
        const char* data = 0;

        if ( type_name && *type_name == '\0' )
            type_name = 0;

        struct_flags = (struct_flags & (FileNode::TYPE_MASK|FileNode::FLOW)) | FileNode::EMPTY;
        if( !FileNode::isCollection(struct_flags))
            CV_Error( CV_StsBadArg,
                     "Some collection type - FileNode::SEQ or FileNode::MAP, must be specified" );

        if (type_name && memcmp(type_name, "binary", 6) == 0)
        {
            /* reset struct flag. in order not to print ']' */
            struct_flags = FileNode::SEQ;
            sprintf(buf, "!!binary |");
            data = buf;
        }
        else if( FileNode::isFlow(struct_flags))
        {
            char c = FileNode::isMap(struct_flags) ? '{' : '[';
            struct_flags |= FileNode::FLOW;

            if( type_name )
                sprintf( buf, "!!%s %c", type_name, c );
            else
            {
                buf[0] = c;
                buf[1] = '\0';
            }
            data = buf;
        }
        else if( type_name )
        {
            sprintf( buf, "!!%s", type_name );
            data = buf;
        }

        writeScalar( key, data );

        FStructData fsd;
        fsd.indent = parent.indent;
        fsd.flags = struct_flags;

        if( !FileNode::isFlow(parent.flags) )
            fsd.indent += CV_YML_INDENT + FileNode::isFlow(struct_flags);

        return fsd;
    }