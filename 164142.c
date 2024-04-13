htmlCtxtReadFile(htmlParserCtxtPtr ctxt, const char *filename,
                const char *encoding, int options)
{
    xmlParserInputPtr stream;

    if (filename == NULL)
        return (NULL);
    if (ctxt == NULL)
        return (NULL);
    xmlInitParser();

    htmlCtxtReset(ctxt);

    stream = xmlLoadExternalEntity(filename, NULL, ctxt);
    if (stream == NULL) {
        return (NULL);
    }
    inputPush(ctxt, stream);
    return (htmlDoRead(ctxt, NULL, encoding, options, 1));
}