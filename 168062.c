nm_utils_format_con_diff_for_audit(GHashTable *diff)
{
    GHashTable *   setting_diff;
    char *         setting_name, *prop_name;
    GHashTableIter iter, iter2;
    GString *      str;

    str = g_string_sized_new(32);
    g_hash_table_iter_init(&iter, diff);

    while (g_hash_table_iter_next(&iter, (gpointer *) &setting_name, (gpointer *) &setting_diff)) {
        if (!setting_diff)
            continue;

        g_hash_table_iter_init(&iter2, setting_diff);

        while (g_hash_table_iter_next(&iter2, (gpointer *) &prop_name, NULL))
            g_string_append_printf(str, "%s.%s,", setting_name, prop_name);
    }

    if (str->len)
        str->str[str->len - 1] = '\0';

    return g_string_free(str, FALSE);
}