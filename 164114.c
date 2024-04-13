htmlCreateFileParserCtxt(const char *filename, const char *encoding)
{
    htmlParserCtxtPtr ctxt;
    htmlParserInputPtr inputStream;
    char *canonicFilename;
    /* htmlCharEncoding enc; */
    xmlChar *content, *content_line = (xmlChar *) "charset=";

    if (filename == NULL)
        return(NULL);

    ctxt = htmlNewParserCtxt();
    if (ctxt == NULL) {
	return(NULL);
    }
    canonicFilename = (char *) xmlCanonicPath((const xmlChar *) filename);
    if (canonicFilename == NULL) {
#ifdef LIBXML_SAX1_ENABLED
	if (xmlDefaultSAXHandler.error != NULL) {
	    xmlDefaultSAXHandler.error(NULL, "out of memory\n");
	}
#endif
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    inputStream = xmlLoadExternalEntity(canonicFilename, NULL, ctxt);
    xmlFree(canonicFilename);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }

    inputPush(ctxt, inputStream);

    /* set encoding */
    if (encoding) {
        size_t l = strlen(encoding);

	if (l < 1000) {
	    content = xmlMallocAtomic (xmlStrlen(content_line) + l + 1);
	    if (content) {
		strcpy ((char *)content, (char *)content_line);
		strcat ((char *)content, (char *)encoding);
		htmlCheckEncoding (ctxt, content);
		xmlFree (content);
	    }
	}
    }

    return(ctxt);
}