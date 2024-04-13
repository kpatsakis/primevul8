htmlReadDoc(const xmlChar * cur, const char *URL, const char *encoding, int options)
{
    htmlParserCtxtPtr ctxt;

    if (cur == NULL)
        return (NULL);

    xmlInitParser();
    ctxt = htmlCreateDocParserCtxt(cur, NULL);
    if (ctxt == NULL)
        return (NULL);
    return (htmlDoRead(ctxt, URL, encoding, options, 0));
}