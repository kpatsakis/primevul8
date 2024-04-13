    bool getBase64Row(char* ptr, int /*indent*/, char* &beg, char* &end)
    {
        beg = end = ptr = skipSpaces(ptr, CV_XML_INSIDE_TAG);
        if( !ptr || !*ptr )
            return false;

        // closing XML tag
        if ( *beg == '<' )
            return false;

        // find end of the row
        while( cv_isprint(*ptr) )
            ++ptr;
        if ( *ptr == '\0' )
            CV_PARSE_ERROR_CPP( "Unexpected end of line" );

        end = ptr;
        return true;
    }