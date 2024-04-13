    char* parseTag( char* ptr, std::string& tag_name,
                    std::string& type_name, int& tag_type )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid tag input");

        if( *ptr == '\0' )
            CV_PARSE_ERROR_CPP( "Unexpected end of the stream" );

        if( *ptr != '<' )
            CV_PARSE_ERROR_CPP( "Tag should start with \'<\'" );

        ptr++;
        CV_PERSISTENCE_CHECK_END_OF_BUFFER_BUG_CPP();

        if( cv_isalnum(*ptr) || *ptr == '_' )
            tag_type = CV_XML_OPENING_TAG;
        else if( *ptr == '/' )
        {
            tag_type = CV_XML_CLOSING_TAG;
            ptr++;
        }
        else if( *ptr == '?' )
        {
            tag_type = CV_XML_HEADER_TAG;
            ptr++;
        }
        else if( *ptr == '!' )
        {
            tag_type = CV_XML_DIRECTIVE_TAG;
            assert( ptr[1] != '-' || ptr[2] != '-' );
            ptr++;
        }
        else
            CV_PARSE_ERROR_CPP( "Unknown tag type" );

        tag_name.clear();
        type_name.clear();

        for(;;)
        {
            char c, *endptr;
            if( !cv_isalpha(*ptr) && *ptr != '_' )
                CV_PARSE_ERROR_CPP( "Name should start with a letter or underscore" );

            endptr = ptr - 1;
            do c = *++endptr;
            while( cv_isalnum(c) || c == '_' || c == '-' );

            std::string attrname(ptr, (size_t)(endptr - ptr));
            ptr = endptr;
            CV_PERSISTENCE_CHECK_END_OF_BUFFER_BUG_CPP();

            if( tag_name.empty() )
                tag_name = attrname;
            else
            {
                if( tag_type == CV_XML_CLOSING_TAG )
                    CV_PARSE_ERROR_CPP( "Closing tag should not contain any attributes" );

                if( *ptr != '=' )
                {
                    ptr = skipSpaces( ptr, CV_XML_INSIDE_TAG );
                    if (!ptr)
                        CV_PARSE_ERROR_CPP("Invalid attribute");
                    if( *ptr != '=' )
                        CV_PARSE_ERROR_CPP( "Attribute name should be followed by \'=\'" );
                }

                c = *++ptr;
                if( c != '\"' && c != '\'' )
                {
                    ptr = skipSpaces( ptr, CV_XML_INSIDE_TAG );
                    if( *ptr != '\"' && *ptr != '\'' )
                        CV_PARSE_ERROR_CPP( "Attribute value should be put into single or double quotes" );
                }

                char quote = *ptr++;
                endptr = ptr;
                for(;;)
                {
                    c = *endptr++;
                    if( c == quote )
                        break;
                    if( c == '\0' )
                        CV_PARSE_ERROR_CPP( "Unexpected end of line" );
                }

                if( attrname == "type_id" )
                {
                    CV_Assert( type_name.empty() );
                    type_name = std::string(ptr, (size_t)(endptr - 1 - ptr));
                }

                ptr = endptr;
            }

            c = *ptr;
            bool have_space = cv_isspace(c) || c == '\0';

            if( c != '>' )
            {
                ptr = skipSpaces( ptr, CV_XML_INSIDE_TAG );
                if (!ptr)
                    CV_PARSE_ERROR_CPP("Invalid input");
                c = *ptr;
            }

            if( c == '>' )
            {
                if( tag_type == CV_XML_HEADER_TAG )
                    CV_PARSE_ERROR_CPP( "Invalid closing tag for <?xml ..." );
                ptr++;
                break;
            }
            else if( c == '?' && tag_type == CV_XML_HEADER_TAG )
            {
                if( ptr[1] != '>'  )  // FIXIT ptr[1] - out of bounds read without check
                    CV_PARSE_ERROR_CPP( "Invalid closing tag for <?xml ..." );
                ptr += 2;
                break;
            }
            else if( c == '/' && ptr[1] == '>' && tag_type == CV_XML_OPENING_TAG )  // FIXIT ptr[1] - out of bounds read without check
            {
                tag_type = CV_XML_EMPTY_TAG;
                ptr += 2;
                break;
            }

            if( !have_space )
                CV_PARSE_ERROR_CPP( "There should be space between attributes" );
        }

        return ptr;
    }