nm_utils_g_value_set_strv(GValue *value, GPtrArray *strings)
{
    char **strv;
    guint  i;

    strv = g_new(char *, strings->len + 1);
    for (i = 0; i < strings->len; i++) {
        nm_assert(strings->pdata[i]);
        strv[i] = g_strdup(strings->pdata[i]);
    }
    strv[i] = NULL;

    g_value_take_boxed(value, strv);
}