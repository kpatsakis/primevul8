ASS_Track *ass_read_memory(ASS_Library *library, char *buf,
                           size_t bufsize, char *codepage)
{
    ASS_Track *track;
    int copied = 0;

    if (!buf)
        return 0;

#ifdef CONFIG_ICONV
    if (codepage) {
        buf = sub_recode(library, buf, bufsize, codepage);
        if (!buf)
            return 0;
        else
            copied = 1;
    }
#endif
    if (!copied) {
        char *newbuf = malloc(bufsize + 1);
        if (!newbuf)
            return 0;
        memcpy(newbuf, buf, bufsize);
        newbuf[bufsize] = '\0';
        buf = newbuf;
    }
    track = parse_memory(library, buf);
    free(buf);
    if (!track)
        return 0;

    ass_msg(library, MSGL_INFO, "Added subtitle file: "
            "<memory> (%d styles, %d events)",
            track->n_styles, track->n_events);
    return track;
}