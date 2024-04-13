    FStructData startWriteStruct(const FStructData& parent, const char* key,
                                 int struct_flags, const char* type_name=0)
    {
        std::vector<std::string> attrlist;
        if( type_name && *type_name )
        {
            attrlist.push_back("type_id");
            attrlist.push_back(type_name);
        }

        writeTag( key, CV_XML_OPENING_TAG, attrlist );

        FStructData current_struct;
        current_struct.tag = key ? std::string(key) : std::string();
        current_struct.flags = struct_flags;
        current_struct.indent = parent.indent + CV_XML_INDENT;

        return current_struct;
    }