int ass_read_styles(ASS_Track *track, char *fname, char *codepage)
{
    char *buf;
    ParserState old_state;
    size_t sz;

    buf = read_file(track->library, fname, &sz);
    if (!buf)
        return 1;
#ifdef CONFIG_ICONV
    if (codepage) {
        char *tmpbuf;
        tmpbuf = sub_recode(track->library, buf, sz, codepage);
        free(buf);
        buf = tmpbuf;
    }
    if (!buf)
        return 1;
#endif

    old_state = track->parser_priv->state;
    track->parser_priv->state = PST_STYLES;
    process_text(track, buf);
    free(buf);
    track->parser_priv->state = old_state;

    return 0;
}