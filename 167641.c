xsltNumberFormatDecimal(xmlBufferPtr buffer,
			double number,
			int digit_zero,
			int width,
			int digitsPerGroup,
			int groupingCharacter,
			int groupingCharacterLen)
{
    /*
     * This used to be
     *  xmlChar temp_string[sizeof(double) * CHAR_BIT * sizeof(xmlChar) + 4];
     * which would be length 68 on x86 arch.  It was changed to be a longer,
     * fixed length in order to try to cater for (reasonable) UTF8
     * separators and numeric characters.  The max UTF8 char size will be
     * 6 or less, so the value used [500] should be *much* larger than needed
     */
    xmlChar temp_string[500];
    xmlChar *pointer;
    xmlChar temp_char[6];
    int i;
    int val;
    int len;

    /* Build buffer from back */
    pointer = &temp_string[sizeof(temp_string)] - 1;	/* last char */
    *pointer = 0;
    i = 0;
    while (pointer > temp_string) {
	if ((i >= width) && (fabs(number) < 1.0))
	    break; /* for */
	if ((i > 0) && (groupingCharacter != 0) &&
	    (digitsPerGroup > 0) &&
	    ((i % digitsPerGroup) == 0)) {
	    if (pointer - groupingCharacterLen < temp_string) {
	        i = -1;		/* flag error */
		break;
	    }
	    pointer -= groupingCharacterLen;
	    xmlCopyCharMultiByte(pointer, groupingCharacter);
	}

	val = digit_zero + (int)fmod(number, 10.0);
	if (val < 0x80) {			/* shortcut if ASCII */
	    if (pointer <= temp_string) {	/* Check enough room */
	        i = -1;
		break;
	    }
	    *(--pointer) = val;
	}
	else {
	/*
	 * Here we have a multibyte character.  It's a little messy,
	 * because until we generate the char we don't know how long
	 * it is.  So, we generate it into the buffer temp_char, then
	 * copy from there into temp_string.
	 */
	    len = xmlCopyCharMultiByte(temp_char, val);
	    if ( (pointer - len) < temp_string ) {
	        i = -1;
		break;
	    }
	    pointer -= len;
	    memcpy(pointer, temp_char, len);
	}
	number /= 10.0;
	++i;
    }
    if (i < 0)
        xsltGenericError(xsltGenericErrorContext,
		"xsltNumberFormatDecimal: Internal buffer size exceeded\n");
    xmlBufferCat(buffer, pointer);
}