htmlReadIO(xmlInputReadCallback ioread, xmlInputCloseCallback ioclose,
          void *ioctx, const char *URL, const char *encoding, int options)
{
    htmlParserCtxtPtr ctxt;
    xmlParserInputBufferPtr input;
    xmlParserInputPtr stream;

    if (ioread == NULL)
        return (NULL);
    xmlInitParser();

    input = xmlParserInputBufferCreateIO(ioread, ioclose, ioctx,
                                         XML_CHAR_ENCODING_NONE);
    if (input == NULL) {
        if (ioclose != NULL)
            ioclose(ioctx);
        return (NULL);
    }
    ctxt = htmlNewParserCtxt();
    if (ctxt == NULL) {
        xmlFreeParserInputBuffer(input);
        return (NULL);
    }
    stream = xmlNewIOInputStream(ctxt, input, XML_CHAR_ENCODING_NONE);
    if (stream == NULL) {
        xmlFreeParserInputBuffer(input);
	xmlFreeParserCtxt(ctxt);
        return (NULL);
    }
    inputPush(ctxt, stream);
    return (htmlDoRead(ctxt, URL, encoding, options, 0));
}