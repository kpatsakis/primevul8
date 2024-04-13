ASS_Track *ass_read_file(ASS_Library *library, char *fname,
                         char *codepage)
{
    char *buf;
    ASS_Track *track;
    size_t bufsize;

    buf = read_file_recode(library, fname, codepage, &bufsize);
    if (!buf)
        return 0;
    track = parse_memory(library, buf);
    free(buf);
    if (!track)
        return 0;

    track->name = strdup(fname);

    ass_msg(library, MSGL_INFO,
            "Added subtitle file: '%s' (%d styles, %d events)",
            fname, track->n_styles, track->n_events);

    return track;
}