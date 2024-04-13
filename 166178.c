    char* parseKey( char* ptr, FileNode& collection, FileNode& value_placeholder )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid input");

        if( *ptr != '"' )
            CV_PARSE_ERROR_CPP( "Key must start with \'\"\'" );

        char * beg = ptr + 1;

        do {
            ++ptr;
            CV_PERSISTENCE_CHECK_END_OF_BUFFER_BUG_CPP();
        } while( cv_isprint(*ptr) && *ptr != '"' );

        if( *ptr != '"' )
            CV_PARSE_ERROR_CPP( "Key must end with \'\"\'" );

        const char * end = ptr;
        ptr++;
        ptr = skipSpaces( ptr );
        if( !ptr || !*ptr )
            return 0;

        if( *ptr != ':' )
            CV_PARSE_ERROR_CPP( "Missing \':\' between key and value" );

        /* [beg, end) */
        if( end <= beg )
            CV_PARSE_ERROR_CPP( "Key is empty" );

        value_placeholder = fs->addNode(collection, std::string(beg, (size_t)(end - beg)), FileNode::NONE);
        return ++ptr;
    }