get_max_rate_vht(const guint8 *bytes, guint len, guint32 *out_maxrate)
{
    guint32 mcs, m;
    guint8  vht_cap, tx_map;

    /* https://tda802dot11.blogspot.it/2014/10/vht-capabilities-element-vht.html
     * http://chimera.labs.oreilly.com/books/1234000001739/ch03.html#management_frames */

    if (len != 12)
        return FALSE;

    vht_cap = bytes[0];
    tx_map  = bytes[8];

    /* Check for mcs rates 8 and 9 support */
    if (tx_map & 0x2a)
        mcs = 9;
    else if (tx_map & 0x15)
        mcs = 8;
    else
        mcs = 7;

    /* Check for 160Mhz wide channel support and
     * spatial stream support */
    if (vht_cap & (1 << 2)) {
        if (tx_map & 0x30)
            m = get_max_rate_vht_160_ss3(mcs);
        else if (tx_map & 0x0C)
            m = get_max_rate_vht_160_ss2(mcs);
        else
            m = get_max_rate_vht_160_ss1(mcs);
    } else {
        if (tx_map & 0x30)
            m = get_max_rate_vht_80_ss3(mcs);
        else if (tx_map & 0x0C)
            m = get_max_rate_vht_80_ss2(mcs);
        else
            m = get_max_rate_vht_80_ss1(mcs);
    }

    *out_maxrate = m;
    return TRUE;
}