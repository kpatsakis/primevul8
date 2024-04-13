int gx_device_delete_output_file(const gx_device * dev, const char *fname)
{
    gs_parsed_file_name_t parsed;
    const char *fmt;
    char *pfname = (char *)gs_alloc_bytes(dev->memory, gp_file_name_sizeof, "gx_device_delete_output_file(pfname)");
    int code;

    if (pfname == NULL) {
        code = gs_note_error(gs_error_VMerror);
	goto done;
    }

    code = gx_parse_output_file_name(&parsed, &fmt, fname, strlen(fname),
                                         dev->memory);
    if (code < 0) {
        goto done;
    }

    if (parsed.iodev && !strcmp(parsed.iodev->dname, "%stdout%"))
        goto done;

    if (fmt) {						/* filename includes "%nnd" */
        long count1 = dev->PageCount + 1;

        while (*fmt != 'l' && *fmt != '%')
            --fmt;
        if (*fmt == 'l')
            gs_sprintf(pfname, parsed.fname, count1);
        else
            gs_sprintf(pfname, parsed.fname, (int)count1);
    } else if (parsed.len && strchr(parsed.fname, '%'))	/* filename with "%%" but no "%nnd" */
        gs_sprintf(pfname, parsed.fname);
    else
        pfname[0] = 0; /* 0 to use "fname", not "pfname" */
    if (pfname[0]) {
        parsed.fname = pfname;
        parsed.len = strlen(parsed.fname);
    }
    if (parsed.iodev)
        code = parsed.iodev->procs.delete_file((gx_io_device *)(&parsed.iodev), (const char *)parsed.fname);
    else
        code = gs_note_error(gs_error_invalidfileaccess);

done:
    if (pfname != NULL)
        gs_free_object(dev->memory, pfname, "gx_device_delete_output_file(pfname)");

    return(code);
}