gx_outputfile_is_separate_pages(const char *fname, gs_memory_t *memory)
{
    const char *fmt;
    gs_parsed_file_name_t parsed;
    int code = gx_parse_output_file_name(&parsed, &fmt, fname,
                                         strlen(fname), memory);

    return (code >= 0 && fmt != 0);
}