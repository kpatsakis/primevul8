_log_connection_get_property(NMSetting *setting, const char *name)
{
    GValue val = G_VALUE_INIT;
    char * s;

    g_return_val_if_fail(setting, NULL);

    if (!NM_IS_SETTING_VPN(setting) && nm_setting_get_secret_flags(setting, name, NULL, NULL))
        return g_strdup("****");

    if (!_nm_setting_get_property(setting, name, &val))
        return g_strdup("<unknown>");

    if (G_VALUE_HOLDS_STRING(&val)) {
        const char *val_s;

        val_s = g_value_get_string(&val);
        if (!val_s) {
            /* for NULL, we want to return the unquoted string "NULL". */
            s = g_strdup("NULL");
        } else {
            char *escaped = g_strescape(val_s, "'");

            s = g_strdup_printf("'%s'", escaped);
            g_free(escaped);
        }
    } else {
        s = g_strdup_value_contents(&val);
        if (s == NULL)
            s = g_strdup("NULL");
        else {
            char *escaped = g_strescape(s, "'");

            g_free(s);
            s = escaped;
        }
    }
    g_value_unset(&val);
    return s;
}