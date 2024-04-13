nm_wifi_utils_parse_ies(const guint8 *bytes,
                        gsize         len,
                        guint32 *     out_max_rate,
                        gboolean *    out_metered,
                        gboolean *    out_owe_transition_mode)
{
    guint8  id, elem_len;
    guint32 m;

    NM_SET_OUT(out_max_rate, 0);
    NM_SET_OUT(out_metered, FALSE);
    NM_SET_OUT(out_owe_transition_mode, FALSE);

    while (len) {
        if (len < 2)
            break;

        id       = *bytes++;
        elem_len = *bytes++;
        len -= 2;

        if (elem_len > len)
            break;

        switch (id) {
        case WLAN_EID_HT_CAPABILITY:
            if (out_max_rate) {
                if (get_max_rate_ht(bytes, elem_len, &m))
                    *out_max_rate = NM_MAX(*out_max_rate, m);
            }
            break;
        case WLAN_EID_VHT_CAPABILITY:
            if (out_max_rate) {
                if (get_max_rate_vht(bytes, elem_len, &m))
                    *out_max_rate = NM_MAX(*out_max_rate, m);
            }
            break;
        case WLAN_EID_VENDOR_SPECIFIC:
            if (len == 8 && bytes[0] == 0x00 /* OUI: Microsoft */
                && bytes[1] == 0x50 && bytes[2] == 0xf2
                && bytes[3] == 0x11)                     /* OUI type: Network cost */
                NM_SET_OUT(out_metered, (bytes[7] > 1)); /* Cost level > 1 */
            if (elem_len >= 10 && bytes[0] == 0x50       /* OUI: WiFi Alliance */
                && bytes[1] == 0x6f && bytes[2] == 0x9a
                && bytes[3] == 0x1c) /* OUI type: OWE Transition Mode */
                NM_SET_OUT(out_owe_transition_mode, TRUE);
            break;
        }

        len -= elem_len;
        bytes += elem_len;
    }
}