static int process_text(ASS_Track *track, char *str)
{
    char *p = str;
    while (1) {
        char *q;
        while (1) {
            if ((*p == '\r') || (*p == '\n'))
                ++p;
            else if (p[0] == '\xef' && p[1] == '\xbb' && p[2] == '\xbf')
                p += 3;         // U+FFFE (BOM)
            else
                break;
        }
        for (q = p; ((*q != '\0') && (*q != '\r') && (*q != '\n')); ++q) {
        };
        if (q == p)
            break;
        if (*q != '\0')
            *(q++) = '\0';
        process_line(track, p);
        if (*q == '\0')
            break;
        p = q;
    }
    // there is no explicit end-of-font marker in ssa/ass
    if (track->parser_priv->fontname)
        decode_font(track);
    return 0;
}