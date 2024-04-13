gx_device_close_output_file(const gx_device * dev, const char *fname,
                            FILE *file)
{
    gs_parsed_file_name_t parsed;
    const char *fmt;
    int code = gx_parse_output_file_name(&parsed, &fmt, fname, strlen(fname),
                                         dev->memory);

    if (code < 0)
        return code;
    if (parsed.iodev) {
        if (!strcmp(parsed.iodev->dname, "%stdout%"))
            return 0;
        /* NOTE: fname is unsubstituted if the name has any %nnd formats. */
        if (parsed.iodev != iodev_default(dev->memory))
            return parsed.iodev->procs.fclose(parsed.iodev, file);
    }
    gp_close_printer(dev->memory, file, (parsed.fname ? parsed.fname : fname));
    return 0;
}