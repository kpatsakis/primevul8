htmlCtxtReadMemory(htmlParserCtxtPtr ctxt, const char *buffer, int size,
                  const char *URL, const char *encoding, int options)
{
    xmlParserInputBufferPtr input;
    xmlParserInputPtr stream;

    if (ctxt == NULL)
        return (NULL);
    if (buffer == NULL)
        return (NULL);
    xmlInitParser();

    htmlCtxtReset(ctxt);

    input = xmlParserInputBufferCreateMem(buffer, size, XML_CHAR_ENCODING_NONE);
    if (input == NULL) {
	return(NULL);
    }

    stream = xmlNewIOInputStream(ctxt, input, XML_CHAR_ENCODING_NONE);
    if (stream == NULL) {
	xmlFreeParserInputBuffer(input);
	return(NULL);
    }

    inputPush(ctxt, stream);
    return (htmlDoRead(ctxt, URL, encoding, options, 1));
}