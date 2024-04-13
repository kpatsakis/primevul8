gx_parse_output_file_name(gs_parsed_file_name_t *pfn, const char **pfmt,
                          const char *fname, uint fnlen, gs_memory_t *memory)
{
    int code;

    *pfmt = 0;
    pfn->memory = 0;
    pfn->iodev = NULL;
    pfn->fname = NULL;		/* irrelevant since length = 0 */
    pfn->len = 0;
    if (fnlen == 0)  		/* allow null name */
        return 0;
    /*
     * If the file name begins with a %, it might be either an IODevice
     * or a %nnd format.  Check (carefully) for this case.
     */
    code = gs_parse_file_name(pfn, fname, fnlen, memory);
    if (code < 0) {
        if (fname[0] == '%') {
            /* not a recognized iodev -- may be a leading format descriptor */
            pfn->len = fnlen;
            pfn->fname = fname;
            code = gx_parse_output_format(pfn, pfmt);
        }
        if (code < 0)
            return code;
    }
    if (!pfn->iodev) {
        if ( (pfn->len == 1) && (pfn->fname[0] == '-') ) {
            pfn->iodev = gs_findiodevice(memory, (const byte *)"%stdout", 7);
            pfn->fname = NULL;
        } else if (pfn->fname[0] == '|') {
            pfn->iodev = gs_findiodevice(memory, (const byte *)"%pipe", 5);
            pfn->fname++, pfn->len--;
        } else
            pfn->iodev = iodev_default(memory);
        if (!pfn->iodev)
            return_error(gs_error_undefinedfilename);
    }
    if (!pfn->fname)
        return 0;
    code = gx_parse_output_format(pfn, pfmt);
    if (code < 0)
        return code;
    if (strlen(pfn->iodev->dname) + pfn->len + code >= gp_file_name_sizeof)
        return_error(gs_error_undefinedfilename);
    return 0;
}