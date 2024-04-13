    char* parseMap( char* ptr, FileNode& node )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("ptr is NULL");

        if ( *ptr != '{' )
            CV_PARSE_ERROR_CPP( "'{' - left-brace of map is missing" );
        else
            ptr++;

        fs->convertToCollection(FileNode::MAP, node);

        for( ;; )
        {
            ptr = skipSpaces( ptr );
            if( !ptr || !*ptr )
                break;

            if ( *ptr == '"' )
            {
                FileNode child;
                ptr = parseKey( ptr, node, child );
                if( !ptr || !*ptr )
                    break;
                ptr = skipSpaces( ptr );
                if( !ptr || !*ptr )
                    break;

                if ( *ptr == '[' )
                    ptr = parseSeq( ptr, child );
                else if ( *ptr == '{' )
                    ptr = parseMap( ptr, child );
                else
                    ptr = parseValue( ptr, child );
            }

            ptr = skipSpaces( ptr );
            if( !ptr || !*ptr )
                break;

            if ( *ptr == ',' )
                ptr++;
            else if ( *ptr == '}' )
                break;
            else
                CV_PARSE_ERROR_CPP( "Unexpected character" );
        }

        if (!ptr)
            CV_PARSE_ERROR_CPP("ptr is NULL");

        if ( *ptr != '}' )
            CV_PARSE_ERROR_CPP( "'}' - right-brace of map is missing" );
        else
            ptr++;

        fs->finalizeCollection(node);
        return ptr;
    }