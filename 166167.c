    char* parseValue( char* ptr, FileNode& node, int min_indent, bool is_parent_flow )
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid input");

        char* endptr = 0;
        char c = ptr[0], d = ptr[1];
        int value_type = FileNode::NONE;
        int len;
        bool is_binary_string = false;
        bool is_user = false;

        if( c == '!' ) // handle explicit type specification
        {
            if( d == '!' || d == '^' )
            {
                ptr++;
                is_user = true;
                //value_type |= FileNode::USER;
            }
            if ( d == '<') //support of full type heading from YAML 1.2
            {
                const char* yamlTypeHeading = "<tag:yaml.org,2002:";
                const size_t headingLenght = strlen(yamlTypeHeading);

                char* typeEndPtr = ++ptr;

                do d = *++typeEndPtr;
                while( cv_isprint(d) && d != ' ' && d != '>' );

                if ( d == '>' && (size_t)(typeEndPtr - ptr) > headingLenght )
                {
                    if ( memcmp(ptr, yamlTypeHeading, headingLenght) == 0 )
                    {
                        *typeEndPtr = ' ';
                        ptr += headingLenght - 1;
                        is_user = true;
                        //value_type |= FileNode::USER;
                    }
                }
            }

            endptr = ptr++;
            do d = *++endptr;
            while( cv_isprint(d) && d != ' ' );
            len = (int)(endptr - ptr);
            if( len == 0 )
                CV_PARSE_ERROR_CPP( "Empty type name" );
            d = *endptr;
            *endptr = '\0';

            if( len == 3 && !is_user )
            {
                if( memcmp( ptr, "str", 3 ) == 0 )
                    value_type = FileNode::STRING;
                else if( memcmp( ptr, "int", 3 ) == 0 )
                    value_type = FileNode::INT;
                else if( memcmp( ptr, "seq", 3 ) == 0 )
                    value_type = FileNode::SEQ;
                else if( memcmp( ptr, "map", 3 ) == 0 )
                    value_type = FileNode::MAP;
            }
            else if( len == 5 && !is_user )
            {
                if( memcmp( ptr, "float", 5 ) == 0 )
                    value_type = FileNode::REAL;
            }
            else if (len == 6 && is_user)
            {
                if( memcmp( ptr, "binary", 6 ) == 0 ) {
                    value_type = FileNode::SEQ;
                    is_binary_string = true;

                    /* for ignore '|' */

                    /**** operation with endptr ****/
                    *endptr = d;

                    do {
                        d = *++endptr;
                        if (d == '|')
                            break;
                    } while (d == ' ');

                    d = *++endptr;
                    *endptr = '\0';
                }
            }

            *endptr = d;
            ptr = skipSpaces( endptr, min_indent, INT_MAX );
            if (!ptr)
                CV_PARSE_ERROR_CPP("Invalid input");

            c = *ptr;

            if( !is_user )
            {
                if (value_type == FileNode::STRING && c != '\'' && c != '\"')
                    goto force_string;
                if( value_type == FileNode::INT )
                    goto force_int;
                if( value_type == FileNode::REAL )
                    goto force_real;
            }
        }

        if (is_binary_string)
        {
            int indent = static_cast<int>(ptr - fs->bufferStart());
            ptr = fs->parseBase64(ptr, indent, node);
        }
        else if( cv_isdigit(c) ||
                ((c == '-' || c == '+') && (cv_isdigit(d) || d == '.')) ||
                (c == '.' && cv_isalnum(d))) // a number
        {
            endptr = ptr + (c == '-' || c == '+');
            while( cv_isdigit(*endptr) )
                endptr++;
            if( *endptr == '.' || *endptr == 'e' )
            {
            force_real:
                double fval = fs->strtod( ptr, &endptr );
                node.setValue(FileNode::REAL, &fval);
            }
            else
            {
            force_int:
                int ival = (int)strtol( ptr, &endptr, 0 );
                node.setValue(FileNode::INT, &ival);
            }

            if( !endptr || endptr == ptr )
                CV_PARSE_ERROR_CPP( "Invalid numeric value (inconsistent explicit type specification?)" );

            ptr = endptr;
            CV_PERSISTENCE_CHECK_END_OF_BUFFER_BUG_CPP();
        }
        else if( c == '\'' || c == '\"' ) // an explicit string
        {
            if( c == '\'' )
                for( len = 0; len < CV_FS_MAX_LEN; )
                {
                    c = *++ptr;
                    if( cv_isalnum(c) || (c != '\'' && cv_isprint(c)))
                        buf[len++] = c;
                    else if( c == '\'' )
                    {
                        c = *++ptr;
                        if( c != '\'' )
                            break;
                        buf[len++] = c;
                    }
                    else
                        CV_PARSE_ERROR_CPP( "Invalid character" );
                }
            else
                for( len = 0; len < CV_FS_MAX_LEN; )
                {
                    c = *++ptr;
                    if( cv_isalnum(c) || (c != '\\' && c != '\"' && cv_isprint(c)))
                        buf[len++] = c;
                    else if( c == '\"' )
                    {
                        ++ptr;
                        break;
                    }
                    else if( c == '\\' )
                    {
                        d = *++ptr;
                        if( d == '\'' )
                            buf[len++] = d;
                        else if( d == '\"' || d == '\\' || d == '\'' )
                            buf[len++] = d;
                        else if( d == 'n' )
                            buf[len++] = '\n';
                        else if( d == 'r' )
                            buf[len++] = '\r';
                        else if( d == 't' )
                            buf[len++] = '\t';
                        else if( d == 'x' || (cv_isdigit(d) && d < '8') )
                        {
                            int val, is_hex = d == 'x';
                            c = ptr[3];
                            ptr[3] = '\0';
                            val = (int)strtol( ptr + is_hex, &endptr, is_hex ? 8 : 16 );
                            ptr[3] = c;
                            if( endptr == ptr + is_hex )
                                buf[len++] = 'x';
                            else
                            {
                                buf[len++] = (char)val;
                                ptr = endptr;
                            }
                        }
                    }
                    else
                        CV_PARSE_ERROR_CPP( "Invalid character" );
                }

            if( len >= CV_FS_MAX_LEN )
                CV_PARSE_ERROR_CPP( "Too long string literal" );

            node.setValue(FileNode::STRING, buf, len);
        }
        else if( c == '[' || c == '{' ) // collection as a flow
        {
            int new_min_indent = min_indent + !is_parent_flow;
            int struct_type = c == '{' ? FileNode::MAP : FileNode::SEQ;
            int nelems = 0;

            fs->convertToCollection(struct_type, node);
            d = c == '[' ? ']' : '}';

            for( ++ptr ;; nelems++ )
            {
                FileNode elem;

                ptr = skipSpaces( ptr, new_min_indent, INT_MAX );
                if (!ptr)
                    CV_PARSE_ERROR_CPP("Invalid input");
                if( *ptr == '}' || *ptr == ']' )
                {
                    if( *ptr != d )
                        CV_PARSE_ERROR_CPP( "The wrong closing bracket" );
                    ptr++;
                    break;
                }

                if( nelems != 0 )
                {
                    if( *ptr != ',' )
                        CV_PARSE_ERROR_CPP( "Missing , between the elements" );
                    ptr = skipSpaces( ptr + 1, new_min_indent, INT_MAX );
                    if (!ptr)
                        CV_PARSE_ERROR_CPP("Invalid input");
                }

                if( struct_type == FileNode::MAP )
                {
                    ptr = parseKey( ptr, node, elem );
                    ptr = skipSpaces( ptr, new_min_indent, INT_MAX );
                }
                else
                {
                    if( *ptr == ']' )
                        break;
                    elem = fs->addNode(node, std::string(), FileNode::NONE);
                }
                ptr = parseValue( ptr, elem, new_min_indent, true );
            }
            fs->finalizeCollection(node);
        }
        else
        {
            int indent, struct_type;

            if( is_parent_flow || c != '-' )
            {
                // implicit (one-line) string or nested block-style collection
                if( !is_parent_flow )
                {
                    if( c == '?' )
                        CV_PARSE_ERROR_CPP( "Complex keys are not supported" );
                    if( c == '|' || c == '>' )
                        CV_PARSE_ERROR_CPP( "Multi-line text literals are not supported" );
                }

            force_string:
                endptr = ptr - 1;

                do c = *++endptr;
                while( cv_isprint(c) &&
                      (!is_parent_flow || (c != ',' && c != '}' && c != ']')) &&
                      (is_parent_flow || c != ':' || value_type == FileNode::STRING));

                if( endptr == ptr )
                    CV_PARSE_ERROR_CPP( "Invalid character" );

                if( is_parent_flow || c != ':' )
                {
                    char* str_end = endptr;
                    // strip spaces in the end of string
                    do c = *--str_end;
                    while( str_end > ptr && c == ' ' );
                    str_end++;
                    node.setValue(FileNode::STRING, ptr, (int)(str_end - ptr));
                    ptr = endptr;
                    return ptr;
                }
                struct_type = FileNode::MAP;
            }
            else
                struct_type = FileNode::SEQ;

            fs->convertToCollection( struct_type, node );
            indent = (int)(ptr - fs->bufferStart());

            for(;;)
            {
                FileNode elem;

                if( struct_type == FileNode::MAP )
                {
                    ptr = parseKey( ptr, node, elem );
                }
                else
                {
                    c = *ptr++;
                    if( c != '-' )
                        CV_PARSE_ERROR_CPP( "Block sequence elements must be preceded with \'-\'" );

                    elem = fs->addNode(node, std::string(), FileNode::NONE);
                }
                ptr = skipSpaces( ptr, indent + 1, INT_MAX );
                ptr = parseValue( ptr, elem, indent + 1, false );
                ptr = skipSpaces( ptr, 0, INT_MAX );
                if( ptr - fs->bufferStart() != indent )
                {
                    if( ptr - fs->bufferStart() < indent )
                        break;
                    else
                        CV_PARSE_ERROR_CPP( "Incorrect indentation" );
                }
                if( memcmp( ptr, "...", 3 ) == 0 )
                    break;
            }
            fs->finalizeCollection(node);
        }

        return ptr;
    }