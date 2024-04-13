nm_utils_read_plugin_paths(const char *dirname, const char *prefix)
{
    GDir *      dir;
    GError *    error = NULL;
    const char *item;
    GArray *    paths;
    char **     result;
    guint       i;

    g_return_val_if_fail(dirname, NULL);
    g_return_val_if_fail(prefix, NULL);

    dir = g_dir_open(dirname, 0, &error);
    if (!dir) {
        nm_log_warn(LOGD_CORE,
                    "device plugin: failed to open directory %s: %s",
                    dirname,
                    error->message);
        g_clear_error(&error);
        return NULL;
    }

    paths = g_array_new(FALSE, FALSE, sizeof(struct plugin_info));

    while ((item = g_dir_read_name(dir))) {
        int                errsv;
        struct plugin_info data;

        if (!g_str_has_prefix(item, prefix))
            continue;
        if (!g_str_has_suffix(item, ".so"))
            continue;

        data.path = g_build_filename(dirname, item, NULL);

        if (stat(data.path, &data.st) != 0) {
            errsv = errno;
            nm_log_warn(LOGD_CORE,
                        "plugin: skip invalid file %s (error during stat: %s)",
                        data.path,
                        nm_strerror_native(errsv));
            goto skip;
        }

        if (!nm_utils_validate_plugin(data.path, &data.st, &error)) {
            nm_log_warn(LOGD_CORE, "plugin: skip invalid file %s: %s", data.path, error->message);
            g_clear_error(&error);
            goto skip;
        }

        g_array_append_val(paths, data);
        continue;
skip:
        g_free(data.path);
    }
    g_dir_close(dir);

    /* sort filenames by modification time. */
    g_array_sort(paths, read_device_factory_paths_sort_fcn);

    result = g_new(char *, paths->len + 1);
    for (i = 0; i < paths->len; i++)
        result[i] = g_array_index(paths, struct plugin_info, i).path;
    result[i] = NULL;

    g_array_free(paths, TRUE);
    return result;
}