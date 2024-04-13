htmlReadFd(int fd, const char *URL, const char *encoding, int options)
{
    htmlParserCtxtPtr ctxt;
    xmlParserInputBufferPtr input;
    xmlParserInputPtr stream;

    if (fd < 0)
        return (NULL);
    xmlInitParser();

    xmlInitParser();
    input = xmlParserInputBufferCreateFd(fd, XML_CHAR_ENCODING_NONE);
    if (input == NULL)
        return (NULL);
    ctxt = xmlNewParserCtxt();
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