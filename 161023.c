xmlNewSaveCtxt(const char *encoding, int options)
{
    xmlSaveCtxtPtr ret;

    ret = (xmlSaveCtxtPtr) xmlMalloc(sizeof(xmlSaveCtxt));
    if (ret == NULL) {
	xmlSaveErrMemory("creating saving context");
	return ( NULL );
    }
    memset(ret, 0, sizeof(xmlSaveCtxt));

    if (encoding != NULL) {
        ret->handler = xmlFindCharEncodingHandler(encoding);
	if (ret->handler == NULL) {
	    xmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL, encoding);
            xmlFreeSaveCtxt(ret);
	    return(NULL);
	}
        ret->encoding = xmlStrdup((const xmlChar *)encoding);
	ret->escape = NULL;
    }
    xmlSaveCtxtInit(ret);

    /*
     * Use the options
     */

    /* Re-check this option as it may already have been set */
    if ((ret->options & XML_SAVE_NO_EMPTY) && ! (options & XML_SAVE_NO_EMPTY)) {
	options |= XML_SAVE_NO_EMPTY;
    }

    ret->options = options;
    if (options & XML_SAVE_FORMAT)
        ret->format = 1;
    else if (options & XML_SAVE_WSNONSIG)
        ret->format = 2;

    return(ret);
}