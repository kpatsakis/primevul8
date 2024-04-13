htmlReadFile(const char *filename, const char *encoding, int options)
{
    htmlParserCtxtPtr ctxt;

    xmlInitParser();
    ctxt = htmlCreateFileParserCtxt(filename, encoding);
    if (ctxt == NULL)
        return (NULL);
    return (htmlDoRead(ctxt, NULL, NULL, options, 0));
}