    char* parseSeq( char* ptr, FileNode& node )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP( "ptr is NULL" );

        if ( *ptr != '[' )
            CV_PARSE_ERROR_CPP( "'[' - left-brace of seq is missing" );
        else
            ptr++;

        fs->convertToCollection(FileNode::SEQ, node);

        for (;;)
        {
            ptr = skipSpaces( ptr );
            if( !ptr || !*ptr )
                break;

            if ( *ptr != ']' )
            {
                FileNode child = fs->addNode(node, std::string(), FileNode::NONE );

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
            else if ( *ptr == ']' )
                break;
            else
                CV_PARSE_ERROR_CPP( "Unexpected character" );
        }

        if (!ptr)
            CV_PARSE_ERROR_CPP("ptr is NULL");

        if ( *ptr != ']' )
            CV_PARSE_ERROR_CPP( "']' - right-brace of seq is missing" );
        else
            ptr++;

        fs->finalizeCollection(node);
        return ptr;
    }