    FStructData startWriteStruct( const FStructData& parent, const char* key,
                                  int struct_flags, const char* type_name=0 )
    {
        char data[CV_FS_MAX_LEN + 1024];

        struct_flags = (struct_flags & (FileNode::TYPE_MASK|FileNode::FLOW)) | FileNode::EMPTY;
        if( !FileNode::isCollection(struct_flags))
            CV_Error( CV_StsBadArg,
                     "Some collection type - FileNode::SEQ or FileNode::MAP, must be specified" );

        if( type_name && *type_name == '\0' )
            type_name = 0;

        bool is_real_collection = true;
        if (type_name && memcmp(type_name, "binary", 6) == 0)
        {
            struct_flags = FileNode::STR;
            data[0] = '\0';
            is_real_collection = false;
        }

        if ( is_real_collection )
        {
            char c = FileNode::isMap(struct_flags) ? '{' : '[';
            data[0] = c;
            data[1] = '\0';
        }

        writeScalar( key, data );
        FStructData current_struct("", struct_flags, parent.indent + 4);

        return current_struct;
    }