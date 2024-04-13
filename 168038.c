_log_connection_sort_hashes(NMConnection *connection,
                            NMConnection *diff_base,
                            GHashTable *  connection_diff)
{
    GHashTableIter           iter;
    GArray *                 sorted_hashes;
    LogConnectionSettingData setting_data;

    sorted_hashes = g_array_sized_new(TRUE,
                                      FALSE,
                                      sizeof(LogConnectionSettingData),
                                      g_hash_table_size(connection_diff));

    g_hash_table_iter_init(&iter, connection_diff);
    while (g_hash_table_iter_next(&iter,
                                  (gpointer) &setting_data.name,
                                  (gpointer) &setting_data.setting_diff)) {
        setting_data.setting = nm_connection_get_setting_by_name(connection, setting_data.name);
        setting_data.diff_base_setting =
            diff_base ? nm_connection_get_setting_by_name(diff_base, setting_data.name) : NULL;
        g_assert(setting_data.setting || setting_data.diff_base_setting);
        g_array_append_val(sorted_hashes, setting_data);
    }

    g_array_sort(sorted_hashes, _log_connection_sort_hashes_fcn);
    return sorted_hashes;
}