_stable_id_append(GString *str, const char *substitution)
{
    if (!substitution)
        substitution = "";
    g_string_append_printf(str, "=%zu{%s}", strlen(substitution), substitution);
}