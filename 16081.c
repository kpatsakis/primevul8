gx_device_open_output_file(const gx_device * dev, char *fname,
                           bool binary, bool positionable, FILE ** pfile)
{
    gs_parsed_file_name_t parsed;
    const char *fmt;
    char *pfname = (char *)gs_alloc_bytes(dev->memory, gp_file_name_sizeof, "gx_device_open_output_file(pfname)");
    int code;

    if (pfname == NULL) {
        code = gs_note_error(gs_error_VMerror);
	goto done;
     }

    if (strlen(fname) == 0) {
        code = gs_note_error(gs_error_undefinedfilename);
        emprintf1(dev->memory, "Device '%s' requires an output file but no file was specified.\n", dev->dname);
        goto done;
    }
    code = gx_parse_output_file_name(&parsed, &fmt, fname, strlen(fname), dev->memory);
    if (code < 0) {
        goto done;
    }

    if (parsed.iodev && !strcmp(parsed.iodev->dname, "%stdout%")) {
        if (parsed.fname) {
            code = gs_note_error(gs_error_undefinedfilename);
	    goto done;
	}
        *pfile = dev->memory->gs_lib_ctx->fstdout;
        /* Force stdout to binary. */
        code = gp_setmode_binary(*pfile, true);
	goto done;
    } else if (parsed.iodev && !strcmp(parsed.iodev->dname, "%pipe%")) {
        positionable = false;
    }
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
    if (positionable || (parsed.iodev && parsed.iodev != iodev_default(dev->memory))) {
        char fmode[4];

        if (!parsed.fname) {
            code = gs_note_error(gs_error_undefinedfilename);
	    goto done;
	}
        strcpy(fmode, gp_fmode_wb);
        if (positionable)
            strcat(fmode, "+");
        code = parsed.iodev->procs.gp_fopen(parsed.iodev, parsed.fname, fmode,
                                         pfile, NULL, 0);
        if (code)
            emprintf1(dev->memory,
                      "**** Could not open the file %s .\n",
                      parsed.fname);
    }
    else {
        *pfile = gp_open_printer(dev->memory, (pfname[0] ? pfname : fname), binary);
        if (!(*pfile)) {
            emprintf1(dev->memory, "**** Could not open the file '%s'.\n", (pfname[0] ? pfname : fname));

            code = gs_note_error(gs_error_invalidfileaccess);
        }
    }

done:
    if (pfname != NULL)
        gs_free_object(dev->memory, pfname, "gx_device_open_output_file(pfname)");

    return(code);
}