htmlCtxtReadFd(htmlParserCtxtPtr ctxt, int fd,
              const char *URL, const char *encoding, int options)
{
    xmlParserInputBufferPtr input;
    xmlParserInputPtr stream;

    if (fd < 0)
        return (NULL);
    if (ctxt == NULL)
        return (NULL);
    xmlInitParser();

    htmlCtxtReset(ctxt);


    input = xmlParserInputBufferCreateFd(fd, XML_CHAR_ENCODING_NONE);
    if (input == NULL)
        return (NULL);
    stream = xmlNewIOInputStream(ctxt, input, XML_CHAR_ENCODING_NONE);
    if (stream == NULL) {
        xmlFreeParserInputBuffer(input);
        return (NULL);
    }
    inputPush(ctxt, stream);
    return (htmlDoRead(ctxt, URL, encoding, options, 1));
}