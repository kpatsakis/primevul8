nm_match_spec_split(const char *value)
{
    char *  string_value, *p, *q0, *q;
    GSList *pieces = NULL;
    int     trailing_ws;

    if (!value || !*value)
        return NULL;

    /* Copied from glibs g_key_file_parse_value_as_string() function
     * and adjusted. */

    string_value = g_new(char, strlen(value) + 1);

    p = (char *) value;

    /* skip over leading whitespace */
    while (g_ascii_isspace(*p))
        p++;

    q0 = q      = string_value;
    trailing_ws = 0;
    while (*p) {
        if (*p == '\\') {
            p++;

            switch (*p) {
            case 's':
                *q = ' ';
                break;
            case 'n':
                *q = '\n';
                break;
            case 't':
                *q = '\t';
                break;
            case 'r':
                *q = '\r';
                break;
            case '\\':
                *q = '\\';
                break;
            case '\0':
                break;
            default:
                if (NM_IN_SET(*p, ',', ';'))
                    *q = *p;
                else {
                    *q++ = '\\';
                    *q   = *p;
                }
                break;
            }
            if (*p == '\0')
                break;
            p++;
            trailing_ws = 0;
        } else {
            *q = *p;
            if (*p == '\0')
                break;
            if (g_ascii_isspace(*p)) {
                trailing_ws++;
                p++;
            } else if (NM_IN_SET(*p, ',', ';')) {
                if (q0 < q - trailing_ws)
                    pieces = g_slist_prepend(pieces, g_strndup(q0, (q - q0) - trailing_ws));
                q0 = q + 1;
                p++;
                trailing_ws = 0;
                while (g_ascii_isspace(*p))
                    p++;
            } else
                p++;
        }
        q++;
    }

    *q = '\0';
    if (q0 < q - trailing_ws)
        pieces = g_slist_prepend(pieces, g_strndup(q0, (q - q0) - trailing_ws));
    g_free(string_value);
    return g_slist_reverse(pieces);
}