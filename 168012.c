_log_connection_sort_hashes_fcn(gconstpointer a, gconstpointer b)
{
    const LogConnectionSettingData *v1 = a;
    const LogConnectionSettingData *v2 = b;
    NMSettingPriority               p1, p2;
    NMSetting *                     s1, *s2;

    s1 = v1->setting ?: v1->diff_base_setting;
    s2 = v2->setting ?: v2->diff_base_setting;

    g_assert(s1 && s2);

    p1 = _nm_setting_get_setting_priority(s1);
    p2 = _nm_setting_get_setting_priority(s2);

    if (p1 != p2)
        return p1 > p2 ? 1 : -1;

    return strcmp(v1->name, v2->name);
}