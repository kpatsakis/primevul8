kernel_cmdline_initial_setup_argument (const gchar  *contents,
                                       gchar       **initial_setup_argument,
                                       GError      **error)
{
        GRegex *regex = NULL;
        GMatchInfo *match_info = NULL;
        gchar *match_group = NULL;

        g_return_val_if_fail (initial_setup_argument != NULL, FALSE);

        regex = g_regex_new ("\\bgnome.initial-setup=([^\\s]*)\\b", 0, 0, error);

        if (!regex)
            return FALSE;

        if (!g_regex_match (regex, contents, 0, &match_info)) {
                g_free (match_info);
                g_free (regex);

                g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                             "Could not match gnome.initial-setup= in kernel cmdline");

                return FALSE;
        }

        match_group = g_match_info_fetch (match_info, 1);

        if (!match_group) {
                g_free (match_info);
                g_free (regex);

                g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                             "Could not match gnome.initial-setup= in kernel cmdline");

                return FALSE;
        }

        *initial_setup_argument = match_group;

        g_free (match_info);
        g_free (regex);

        return TRUE;
}