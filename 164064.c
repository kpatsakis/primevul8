htmlCurrentChar(xmlParserCtxtPtr ctxt, int *len) {
    if (ctxt->instate == XML_PARSER_EOF)
	return(0);

    if (ctxt->token != 0) {
	*len = 0;
	return(ctxt->token);
    }
    if (ctxt->charset == XML_CHAR_ENCODING_UTF8) {
	/*
	 * We are supposed to handle UTF8, check it's valid
	 * From rfc2044: encoding of the Unicode values on UTF-8:
	 *
	 * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
	 * 0000 0000-0000 007F   0xxxxxxx
	 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
	 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
	 *
	 * Check for the 0x110000 limit too
	 */
	const unsigned char *cur = ctxt->input->cur;
	unsigned char c;
	unsigned int val;

	c = *cur;
	if (c & 0x80) {
	    if (cur[1] == 0) {
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                cur = ctxt->input->cur;
            }
	    if ((cur[1] & 0xc0) != 0x80)
		goto encoding_error;
	    if ((c & 0xe0) == 0xe0) {

		if (cur[2] == 0) {
		    xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                    cur = ctxt->input->cur;
                }
		if ((cur[2] & 0xc0) != 0x80)
		    goto encoding_error;
		if ((c & 0xf0) == 0xf0) {
		    if (cur[3] == 0) {
			xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
                        cur = ctxt->input->cur;
                    }
		    if (((c & 0xf8) != 0xf0) ||
			((cur[3] & 0xc0) != 0x80))
			goto encoding_error;
		    /* 4-byte code */
		    *len = 4;
		    val = (cur[0] & 0x7) << 18;
		    val |= (cur[1] & 0x3f) << 12;
		    val |= (cur[2] & 0x3f) << 6;
		    val |= cur[3] & 0x3f;
		} else {
		  /* 3-byte code */
		    *len = 3;
		    val = (cur[0] & 0xf) << 12;
		    val |= (cur[1] & 0x3f) << 6;
		    val |= cur[2] & 0x3f;
		}
	    } else {
	      /* 2-byte code */
		*len = 2;
		val = (cur[0] & 0x1f) << 6;
		val |= cur[1] & 0x3f;
	    }
	    if (!IS_CHAR(val)) {
	        htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
				"Char 0x%X out of allowed range\n", val);
	    }
	    return(val);
	} else {
            if ((*ctxt->input->cur == 0) &&
                (ctxt->input->cur < ctxt->input->end)) {
                    htmlParseErrInt(ctxt, XML_ERR_INVALID_CHAR,
				"Char 0x%X out of allowed range\n", 0);
                *len = 1;
                return(' ');
            }
	    /* 1-byte code */
	    *len = 1;
	    return((int) *ctxt->input->cur);
	}
    }
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    if ((int) *ctxt->input->cur < 0x80)
	return((int) *ctxt->input->cur);

    /*
     * Humm this is bad, do an automatic flow conversion
     */
    {
        xmlChar * guess;
        xmlCharEncodingHandlerPtr handler;

        guess = htmlFindEncoding(ctxt);
        if (guess == NULL) {
            xmlSwitchEncoding(ctxt, XML_CHAR_ENCODING_8859_1);
        } else {
            if (ctxt->input->encoding != NULL)
                xmlFree((xmlChar *) ctxt->input->encoding);
            ctxt->input->encoding = guess;
            handler = xmlFindCharEncodingHandler((const char *) guess);
            if (handler != NULL) {
                xmlSwitchToEncoding(ctxt, handler);
            } else {
                htmlParseErr(ctxt, XML_ERR_INVALID_ENCODING,
                             "Unsupported encoding %s", guess, NULL);
            }
        }
        ctxt->charset = XML_CHAR_ENCODING_UTF8;
    }

    return(xmlCurrentChar(ctxt, len));

encoding_error:
    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertized in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    {
        char buffer[150];

	if (ctxt->input->end - ctxt->input->cur >= 4) {
	    snprintf(buffer, 149, "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			    ctxt->input->cur[0], ctxt->input->cur[1],
			    ctxt->input->cur[2], ctxt->input->cur[3]);
	} else {
	    snprintf(buffer, 149, "Bytes: 0x%02X\n", ctxt->input->cur[0]);
	}
	htmlParseErr(ctxt, XML_ERR_INVALID_ENCODING,
		     "Input is not proper UTF-8, indicate encoding !\n",
		     BAD_CAST buffer, NULL);
    }

    ctxt->charset = XML_CHAR_ENCODING_8859_1;
    *len = 1;
    return((int) *ctxt->input->cur);
}