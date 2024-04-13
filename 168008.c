nm_utils_read_link_absolute(const char *link_file, GError **error)
{
    char *ln, *dirname, *ln_abs;

    ln = g_file_read_link(link_file, error);
    if (!ln)
        return NULL;
    if (g_path_is_absolute(ln))
        return ln;

    dirname = g_path_get_dirname(link_file);
    if (!g_path_is_absolute(dirname)) {
        gs_free char *current_dir = g_get_current_dir();

        /* @link_file argument was not an absolute path in the first place.
         * That actually may be a bug, because the CWD is not well defined
         * in most cases. Anyway, apparently we were able to load the file
         * even from a relative path. So, when making the link absolute, we
         * also need to prepend the CWD. */
        ln_abs = g_build_filename(current_dir, dirname, ln, NULL);
    } else
        ln_abs = g_build_filename(dirname, ln, NULL);
    g_free(dirname);
    g_free(ln);
    return ln_abs;
}