    char* parseKey( char* ptr, FileNode& map_node, FileNode& value_placeholder )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid input");

        char c;
        char *endptr = ptr - 1, *saveptr;

        if( *ptr == '-' )
            CV_PARSE_ERROR_CPP( "Key may not start with \'-\'" );

        do c = *++endptr;
        while( cv_isprint(c) && c != ':' );

        if( c != ':' )
            CV_PARSE_ERROR_CPP( "Missing \':\'" );

        saveptr = endptr + 1;
        do c = *--endptr;
        while( c == ' ' );

        ++endptr;
        if( endptr == ptr )
            CV_PARSE_ERROR_CPP( "An empty key" );

        value_placeholder = fs->addNode(map_node, std::string(ptr, endptr - ptr), FileNode::NONE);
        ptr = saveptr;

        return ptr;
    }