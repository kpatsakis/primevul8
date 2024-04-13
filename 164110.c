htmlCtxtReadDoc(htmlParserCtxtPtr ctxt, const xmlChar * cur,
               const char *URL, const char *encoding, int options)
{
    xmlParserInputPtr stream;

    if (cur == NULL)
        return (NULL);
    if (ctxt == NULL)
        return (NULL);
    xmlInitParser();

    htmlCtxtReset(ctxt);

    stream = xmlNewStringInputStream(ctxt, cur);
    if (stream == NULL) {
        return (NULL);
    }
    inputPush(ctxt, stream);
    return (htmlDoRead(ctxt, URL, encoding, options, 1));
}