static int xrefCheck(const char *xref, const char *eof)
{
    const char *q;
    while (xref < eof && (*xref == ' ' || *xref == '\n' || *xref == '\r'))
	xref++;
    if (xref + 4 >= eof)
	return -1;
    if (!memcmp(xref, "xref", 4)) {
	cli_dbgmsg("cli_pdf: found xref\n");
	return 0;
    }
    /* could be xref stream */
    for (q=xref; q+5 < eof; q++) {
	if (!memcmp(q,"/XRef",4)) {
	    cli_dbgmsg("cli_pdf: found /XRef\n");
	    return 0;
	}
    }
    return -1;
}