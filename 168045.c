get_max_rate_ht(const guint8 *bytes, guint len, guint32 *out_maxrate)
{
    guint32       i;
    guint8        ht_cap_info;
    const guint8 *supported_mcs_set;
    guint32       rate;

    /* http://standards.ieee.org/getieee802/download/802.11-2012.pdf
     * https://mrncciew.com/2014/10/19/cwap-ht-capabilities-ie/
     */

    if (len != 26)
        return FALSE;

    ht_cap_info       = bytes[0];
    supported_mcs_set = &bytes[3];
    *out_maxrate      = 0;

    /* Find the maximum supported mcs rate */
    for (i = 0; i <= 76; i++) {
        const unsigned mcs_octet    = i / 8;
        const unsigned MCS_RATE_BIT = 1 << i % 8;

        if (supported_mcs_set[mcs_octet] & MCS_RATE_BIT) {
            /* Check for 40Mhz wide channel support */
            if (ht_cap_info & (1 << 1))
                rate = get_max_rate_ht_40(i);
            else
                rate = get_max_rate_ht_20(i);

            if (rate > *out_maxrate)
                *out_maxrate = rate;
        }
    }

    return TRUE;
}