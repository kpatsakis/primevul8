_log_connection_sort_names(LogConnectionSettingData *setting_data, GArray *sorted_names)
{
    GHashTableIter           iter;
    LogConnectionSettingItem item;
    gpointer                 p;

    g_array_set_size(sorted_names, 0);

    g_hash_table_iter_init(&iter, setting_data->setting_diff);
    while (g_hash_table_iter_next(&iter, (gpointer) &item.item_name, &p)) {
        item.diff_result = GPOINTER_TO_UINT(p);
        g_array_append_val(sorted_names, item);
    }

    g_array_sort(sorted_names, _log_connection_sort_names_fcn);
}