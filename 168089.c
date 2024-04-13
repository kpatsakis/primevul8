nm_match_spec_join(GSList *specs)
{
    const char *p;
    GString *   str;

    str = g_string_new("");

    for (; specs; specs = specs->next) {
        p = specs->data;

        if (!p || !*p)
            continue;

        if (str->len > 0)
            g_string_append_c(str, ',');

        /* escape leading whitespace */
        switch (*p) {
        case ' ':
            g_string_append(str, "\\s");
            p++;
            break;
        case '\t':
            g_string_append(str, "\\t");
            p++;
            break;
        }

        for (; *p; p++) {
            switch (*p) {
            case '\n':
                g_string_append(str, "\\n");
                break;
            case '\r':
                g_string_append(str, "\\r");
                break;
            case '\\':
                g_string_append(str, "\\\\");
                break;
            case ',':
                g_string_append(str, "\\,");
                break;
            case ';':
                g_string_append(str, "\\;");
                break;
            default:
                g_string_append_c(str, *p);
                break;
            }
        }

        /* escape trailing whitespaces */
        switch (str->str[str->len - 1]) {
        case ' ':
            g_string_overwrite(str, str->len - 1, "\\s");
            break;
        case '\t':
            g_string_overwrite(str, str->len - 1, "\\t");
            break;
        }
    }

    return g_string_free(str, FALSE);
}