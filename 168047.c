nm_utils_find_helper(const char *progname, const char *try_first, GError **error)
{
    return nm_utils_file_search_in_paths(progname,
                                         try_first,
                                         NM_PATHS_DEFAULT,
                                         G_FILE_TEST_IS_EXECUTABLE,
                                         NULL,
                                         NULL,
                                         error);
}