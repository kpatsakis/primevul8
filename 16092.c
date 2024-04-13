gx_parse_output_format(gs_parsed_file_name_t *pfn, const char **pfmt)
{
    bool have_format = false, field;
    int width[2], int_width = sizeof(int) * 3, w = 0;
    uint i;

    /* Scan the file name for a format string, and validate it if present. */
    width[0] = width[1] = 0;
    for (i = 0; i < pfn->len; ++i)
        if (pfn->fname[i] == '%') {
            if (i + 1 < pfn->len && pfn->fname[i + 1] == '%') {
                i++;
                continue;
            }
            if (have_format)	/* more than one % */
                return_error(gs_error_undefinedfilename);
            have_format = true;
            field = -1; /* -1..3 for the 5 components of "%[flags][width][.precision][l]type" */
            for (;;)
                if (++i == pfn->len)
                    return_error(gs_error_undefinedfilename);
                else {
                    switch (field) {
                        case -1: /* flags */
                            if (strchr(" #+-", pfn->fname[i]))
                                continue;
                            else
                                field++;
                            /* falls through */
                        default: /* width (field = 0) and precision (field = 1) */
                            if (strchr("0123456789", pfn->fname[i])) {
                                width[field] = width[field] * 10 + pfn->fname[i] - '0';
                                continue;
                            } else if (0 == field && '.' == pfn->fname[i]) {
                                field++;
                                continue;
                            } else
                                field = 2;
                            /* falls through */
                        case 2: /* "long" indicator */
                            field++;
                            if ('l' == pfn->fname[i]) {
                                int_width = sizeof(long) * 3;
                                continue;
                            }
                            /* falls through */
                        case 3: /* type */
                            if (strchr("diuoxX", pfn->fname[i])) {
                                *pfmt = &pfn->fname[i];
                                break;
                            } else
                                return_error(gs_error_undefinedfilename);
                    }
                    break;
                }
        }
    if (have_format) {
        /* Calculate a conservative maximum width. */
        w = max(width[0], width[1]);
        w = max(w, int_width) + 5;
    }
    return w;
}