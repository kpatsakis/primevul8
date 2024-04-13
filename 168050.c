nm_utils_validate_plugin(const char *path, struct stat *st, GError **error)
{
    g_return_val_if_fail(path, FALSE);
    g_return_val_if_fail(st, FALSE);
    g_return_val_if_fail(!error || !*error, FALSE);

    if (!S_ISREG(st->st_mode)) {
        g_set_error_literal(error, NM_UTILS_ERROR, NM_UTILS_ERROR_UNKNOWN, "not a regular file");
        return FALSE;
    }

    if (st->st_uid != 0) {
        g_set_error_literal(error,
                            NM_UTILS_ERROR,
                            NM_UTILS_ERROR_UNKNOWN,
                            "file has invalid owner (should be root)");
        return FALSE;
    }

    if (st->st_mode & (S_IWGRP | S_IWOTH | S_ISUID)) {
        g_set_error_literal(error,
                            NM_UTILS_ERROR,
                            NM_UTILS_ERROR_UNKNOWN,
                            "file has invalid permissions");
        return FALSE;
    }

    return TRUE;
}