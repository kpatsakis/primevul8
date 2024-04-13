nm_utils_kernel_cmdline_match_check(const char *const *proc_cmdline,
                                    const char *const *patterns,
                                    guint              num_patterns,
                                    GError **          error)
{
    gboolean has_optional     = FALSE;
    gboolean has_any_optional = FALSE;
    guint    i;

    for (i = 0; i < num_patterns; i++) {
        const char *element      = patterns[i];
        gboolean    is_inverted  = FALSE;
        gboolean    is_mandatory = FALSE;
        gboolean    match;
        const char *p;

        _pattern_parse(element, &p, &is_inverted, &is_mandatory);

        match = _kernel_cmdline_match(proc_cmdline, p);
        if (is_inverted)
            match = !match;

        if (is_mandatory) {
            if (!match) {
                nm_utils_error_set(error,
                                   NM_UTILS_ERROR_CONNECTION_AVAILABLE_TEMPORARY,
                                   "device does not satisfy match.kernel-command-line property %s",
                                   patterns[i]);
                return FALSE;
            }
        } else {
            has_any_optional = TRUE;
            if (match)
                has_optional = TRUE;
        }
    }

    if (!has_optional && has_any_optional) {
        nm_utils_error_set(error,
                           NM_UTILS_ERROR_CONNECTION_AVAILABLE_TEMPORARY,
                           "device does not satisfy any match.kernel-command-line property");
        return FALSE;
    }

    return TRUE;
}