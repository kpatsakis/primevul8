htmlCheckAutoClose(const xmlChar * newtag, const xmlChar * oldtag)
{
    int i, indx;
    const char **closed = NULL;

    if (htmlStartCloseIndexinitialized == 0)
        htmlInitAutoClose();

    /* inefficient, but not a big deal */
    for (indx = 0; indx < 100; indx++) {
        closed = htmlStartCloseIndex[indx];
        if (closed == NULL)
            return (0);
        if (xmlStrEqual(BAD_CAST * closed, newtag))
            break;
    }

    i = closed - htmlStartClose;
    i++;
    while (htmlStartClose[i] != NULL) {
        if (xmlStrEqual(BAD_CAST htmlStartClose[i], oldtag)) {
            return (1);
        }
        i++;
    }
    return (0);
}