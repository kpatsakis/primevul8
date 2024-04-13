nm_utils_log_connection_diff(NMConnection *connection,
                             NMConnection *diff_base,
                             guint32       level,
                             guint64       domain,
                             const char *  name,
                             const char *  prefix,
                             const char *  dbus_path)
{
    GHashTable *connection_diff = NULL;
    GArray *    sorted_hashes;
    GArray *    sorted_names = NULL;
    int         i, j;
    gboolean    connection_diff_are_same;
    gboolean    print_header = TRUE;
    gboolean    print_setting_header;
    GString *   str1;

    g_return_if_fail(NM_IS_CONNECTION(connection));
    g_return_if_fail(!diff_base || (NM_IS_CONNECTION(diff_base) && diff_base != connection));

    /* For VPN setting types, this is broken, because we cannot (generically) print the content of data/secrets. Bummer... */

    if (!nm_logging_enabled(level, domain))
        return;

    if (!prefix)
        prefix = "";
    if (!name)
        name = "";

    connection_diff_are_same = nm_connection_diff(
        connection,
        diff_base,
        NM_SETTING_COMPARE_FLAG_EXACT | NM_SETTING_COMPARE_FLAG_DIFF_RESULT_NO_DEFAULT,
        &connection_diff);
    if (connection_diff_are_same) {
        const char *t1, *t2;

        t1 = nm_connection_get_connection_type(connection);
        if (diff_base) {
            t2 = nm_connection_get_connection_type(diff_base);
            nm_log(level,
                   domain,
                   NULL,
                   NULL,
                   "%sconnection '%s' (%p/%s/%s%s%s and %p/%s/%s%s%s): no difference",
                   prefix,
                   name,
                   connection,
                   G_OBJECT_TYPE_NAME(connection),
                   NM_PRINT_FMT_QUOTE_STRING(t1),
                   diff_base,
                   G_OBJECT_TYPE_NAME(diff_base),
                   NM_PRINT_FMT_QUOTE_STRING(t2));
        } else {
            nm_log(level,
                   domain,
                   NULL,
                   NULL,
                   "%sconnection '%s' (%p/%s/%s%s%s): no properties set",
                   prefix,
                   name,
                   connection,
                   G_OBJECT_TYPE_NAME(connection),
                   NM_PRINT_FMT_QUOTE_STRING(t1));
        }
        g_assert(!connection_diff);
        return;
    }

    /* FIXME: it doesn't nicely show the content of NMSettingVpn, because nm_connection_diff() does not
     * expand the hash values. */

    sorted_hashes = _log_connection_sort_hashes(connection, diff_base, connection_diff);
    if (sorted_hashes->len <= 0)
        goto out;

    sorted_names = g_array_new(FALSE, FALSE, sizeof(LogConnectionSettingItem));
    str1         = g_string_new(NULL);

    for (i = 0; i < sorted_hashes->len; i++) {
        LogConnectionSettingData *setting_data =
            &g_array_index(sorted_hashes, LogConnectionSettingData, i);

        _log_connection_sort_names(setting_data, sorted_names);
        print_setting_header = TRUE;
        for (j = 0; j < sorted_names->len; j++) {
            char *                    str_conn, *str_diff;
            LogConnectionSettingItem *item =
                &g_array_index(sorted_names, LogConnectionSettingItem, j);

            str_conn = (item->diff_result & NM_SETTING_DIFF_RESULT_IN_A)
                           ? _log_connection_get_property(setting_data->setting, item->item_name)
                           : NULL;
            str_diff =
                (item->diff_result & NM_SETTING_DIFF_RESULT_IN_B)
                    ? _log_connection_get_property(setting_data->diff_base_setting, item->item_name)
                    : NULL;

            if (print_header) {
                GError *    err_verify = NULL;
                const char *t1, *t2;

                t1 = nm_connection_get_connection_type(connection);
                if (diff_base) {
                    t2 = nm_connection_get_connection_type(diff_base);
                    nm_log(level,
                           domain,
                           NULL,
                           NULL,
                           "%sconnection '%s' (%p/%s/%s%s%s < %p/%s/%s%s%s)%s%s%s:",
                           prefix,
                           name,
                           connection,
                           G_OBJECT_TYPE_NAME(connection),
                           NM_PRINT_FMT_QUOTE_STRING(t1),
                           diff_base,
                           G_OBJECT_TYPE_NAME(diff_base),
                           NM_PRINT_FMT_QUOTE_STRING(t2),
                           NM_PRINT_FMT_QUOTED(dbus_path, " [", dbus_path, "]", ""));
                } else {
                    nm_log(level,
                           domain,
                           NULL,
                           NULL,
                           "%sconnection '%s' (%p/%s/%s%s%s):%s%s%s",
                           prefix,
                           name,
                           connection,
                           G_OBJECT_TYPE_NAME(connection),
                           NM_PRINT_FMT_QUOTE_STRING(t1),
                           NM_PRINT_FMT_QUOTED(dbus_path, " [", dbus_path, "]", ""));
                }
                print_header = FALSE;

                if (!nm_connection_verify(connection, &err_verify)) {
                    nm_log(level,
                           domain,
                           NULL,
                           NULL,
                           "%sconnection %p does not verify: %s",
                           prefix,
                           connection,
                           err_verify->message);
                    g_clear_error(&err_verify);
                }
            }
#define _NM_LOG_ALIGN "-25"
            if (print_setting_header) {
                if (diff_base) {
                    if (setting_data->setting && setting_data->diff_base_setting)
                        g_string_printf(str1,
                                        "%p < %p",
                                        setting_data->setting,
                                        setting_data->diff_base_setting);
                    else if (setting_data->diff_base_setting)
                        g_string_printf(str1, "*missing* < %p", setting_data->diff_base_setting);
                    else
                        g_string_printf(str1, "%p < *missing*", setting_data->setting);
                    nm_log(level,
                           domain,
                           NULL,
                           NULL,
                           "%s%"_NM_LOG_ALIGN
                           "s [ %s ]",
                           prefix,
                           setting_data->name,
                           str1->str);
                } else
                    nm_log(level,
                           domain,
                           NULL,
                           NULL,
                           "%s%"_NM_LOG_ALIGN
                           "s [ %p ]",
                           prefix,
                           setting_data->name,
                           setting_data->setting);
                print_setting_header = FALSE;
            }
            g_string_printf(str1, "%s.%s", setting_data->name, item->item_name);
            switch (item->diff_result
                    & (NM_SETTING_DIFF_RESULT_IN_A | NM_SETTING_DIFF_RESULT_IN_B)) {
            case NM_SETTING_DIFF_RESULT_IN_B:
                nm_log(level,
                       domain,
                       NULL,
                       NULL,
                       "%s%"_NM_LOG_ALIGN
                       "s < %s",
                       prefix,
                       str1->str,
                       str_diff ?: "NULL");
                break;
            case NM_SETTING_DIFF_RESULT_IN_A:
                nm_log(level,
                       domain,
                       NULL,
                       NULL,
                       "%s%"_NM_LOG_ALIGN
                       "s = %s",
                       prefix,
                       str1->str,
                       str_conn ?: "NULL");
                break;
            default:
                nm_log(level,
                       domain,
                       NULL,
                       NULL,
                       "%s%"_NM_LOG_ALIGN
                       "s = %s < %s",
                       prefix,
                       str1->str,
                       str_conn ?: "NULL",
                       str_diff ?: "NULL");
                break;
#undef _NM_LOG_ALIGN
            }
            g_free(str_conn);
            g_free(str_diff);
        }
    }

    g_array_free(sorted_names, TRUE);
    g_string_free(str1, TRUE);
out:
    g_hash_table_destroy(connection_diff);
    g_array_free(sorted_hashes, TRUE);
}