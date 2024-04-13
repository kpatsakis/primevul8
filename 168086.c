match_device_s390_subchannels_parse(const char *s390_subchannels,
                                    guint32 *   out_a,
                                    guint32 *   out_b,
                                    guint32 *   out_c)
{
    char      buf[30 + 1];
    const int BUFSIZE = G_N_ELEMENTS(buf) - 1;
    guint     i       = 0;
    char *    pa = NULL, *pb = NULL, *pc = NULL;
    gint64    a, b, c;

    nm_assert(s390_subchannels);
    nm_assert(out_a);
    nm_assert(out_b);
    nm_assert(out_c);

    if (!g_ascii_isxdigit(s390_subchannels[0]))
        return FALSE;

    /* Get the first channel */
    for (i = 0; s390_subchannels[i]; i++) {
        char ch = s390_subchannels[i];

        if (!g_ascii_isxdigit(ch) && ch != '.') {
            if (ch == ',') {
                /* FIXME: currently we consider the first channel and ignore
                 * everything after the first ',' separator. Maybe we should
                 * validate all present channels? */
                break;
            }
            return FALSE; /* Invalid chars */
        }
        if (i >= BUFSIZE)
            return FALSE; /* Too long to be a subchannel */
        buf[i] = ch;
    }
    buf[i] = '\0';

    /* and grab each of its elements, there should be 3 */
    pa = &buf[0];
    pb = strchr(pa, '.');
    if (pb)
        pc = strchr(pb + 1, '.');
    if (!pb || !pc)
        return FALSE;
    *pb++ = '\0';
    *pc++ = '\0';

    a = _nm_utils_ascii_str_to_int64(pa, 16, 0, G_MAXUINT32, -1);
    if (a == -1)
        return FALSE;
    b = _nm_utils_ascii_str_to_int64(pb, 16, 0, G_MAXUINT32, -1);
    if (b == -1)
        return FALSE;
    c = _nm_utils_ascii_str_to_int64(pc, 16, 0, G_MAXUINT32, -1);
    if (c == -1)
        return FALSE;

    *out_a = (guint32) a;
    *out_b = (guint32) b;
    *out_c = (guint32) c;
    return TRUE;
}