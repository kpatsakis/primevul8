nm_utils_cmp_connection_by_autoconnect_priority(NMConnection *a, NMConnection *b)
{
    NMSettingConnection *a_s_con;
    NMSettingConnection *b_s_con;
    int                  a_ap, b_ap;
    gboolean             can_autoconnect;

    if (a == b)
        return 0;
    if (!a)
        return 1;
    if (!b)
        return -1;

    a_s_con = nm_connection_get_setting_connection(a);
    b_s_con = nm_connection_get_setting_connection(b);

    if (!a_s_con)
        return !b_s_con ? 0 : 1;
    if (!b_s_con)
        return -1;

    can_autoconnect = !!nm_setting_connection_get_autoconnect(a_s_con);
    if (can_autoconnect != (!!nm_setting_connection_get_autoconnect(b_s_con)))
        return can_autoconnect ? -1 : 1;

    if (can_autoconnect) {
        a_ap = nm_setting_connection_get_autoconnect_priority(a_s_con);
        b_ap = nm_setting_connection_get_autoconnect_priority(b_s_con);
        if (a_ap != b_ap)
            return (a_ap > b_ap) ? -1 : 1;
    }

    return 0;
}