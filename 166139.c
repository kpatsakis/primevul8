    bool getBase64Row(char* ptr, int indent, char* &beg, char* &end)
    {
        if (!ptr)
            CV_PARSE_ERROR_CPP("Invalid input");

        beg = end = ptr = skipSpaces(ptr, 0, INT_MAX);
        if (!ptr || !*ptr)
            return false; // end of file

        if (ptr - fs->bufferStart() != indent)
            return false; // end of base64 data

        /* find end */
        while(cv_isprint(*ptr)) /* no check for base64 string */
            ++ptr;
        if (*ptr == '\0')
            CV_PARSE_ERROR_CPP("Unexpected end of line");

        end = ptr;
        return true;
    }