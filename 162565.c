unfold_and_compact_mime_header(const char *lines, gint *first_colon_offset)
{
    const char *p = lines;
    char c;
    char *ret, *q;
    char sep_seen = 0; /* Did we see a separator ":;," */
    char lws = FALSE; /* Did we see LWS (incl. folding) */
    gint colon = -1;

    if (! lines) return NULL;

    c = *p;
    ret = (char *)wmem_alloc(wmem_packet_scope(), strlen(lines) + 1);
    q = ret;

    while (c) {
        if (c == ':') {
            lws = FALSE; /* Prevent leading LWS from showing up */
            if (colon == -1) {/* First colon */
                colon = (gint) (q - ret);
            }
            *(q++) = sep_seen = c;
            p++;
        } else if (c == ';' || c == ',' || c == '=') {
            lws = FALSE; /* Prevent leading LWS from showing up */
            *(q++) = sep_seen = c;
            p++;
        } else if (c == ' ' || c == '\t') {
            lws = TRUE;
            p++;
        } else if (c == '\n') {
            lws = FALSE; /* Skip trailing LWS */
            if ((c = *(p+1))) {
                if (c == ' ' || c == '\t') { /* Header unfolding */
                    lws = TRUE;
                    p += 2;
                } else {
                    *q = c = 0; /* Stop */
                }
            }
        } else if (c == '\r') {
            lws = FALSE;
            if ((c = *(p+1))) {
                if (c == '\n') {
                    if ((c = *(p+2))) {
                        if (c == ' ' || c == '\t') { /* Header unfolding */
                            lws = TRUE;
                            p += 3;
                        } else {
                            *q = c = 0; /* Stop */
                        }
                    }
                } else if (c == ' ' || c == '\t') { /* Header unfolding */
                    lws = TRUE;
                    p += 2;
                } else {
                    *q = c = 0; /* Stop */
                }
            }
        } else if (c == '"') { /* Start of quoted-string */
            lws = FALSE;
            *(q++) = c;
            while (c) {
                c = *(q++) = *(++p);
                if (c == '\\') {
                    /* First part of a quoted-pair; copy the other part,
                       without checking if it's a quote */
                    c = *(q++) = *(++p);
                } else {
                    if (c == '"') {
                        p++; /* Skip closing quote */
                        break;
                    }
                }
            }
            /* if already zero terminated now, rewind one char to avoid an "off by one" */
            if(c == 0) {
                q--;
            }
        } else { /* Regular character */
            if (sep_seen) {
                sep_seen = 0;
            } else {
                if (lws) {
                    *(q++) = ' ';
                }
            }
            lws = FALSE;
            *(q++) = c;
            p++; /* OK */
        }

        if (c) {
            c = *p;
        }
    }
    *q = 0;

    *first_colon_offset = colon;
    return (ret);
}