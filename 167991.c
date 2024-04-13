nm_wifi_utils_level_to_quality(int val)
{
    if (val < 0) {
        /* Assume dBm already; rough conversion: best = -40, worst = -100 */
        val = abs(CLAMP(val, -100, -40) + 40); /* normalize to 0 */
        val = 100 - (int) ((100.0 * (double) val) / 60.0);
    } else if (val > 110 && val < 256) {
        /* assume old-style WEXT 8-bit unsigned signal level */
        val -= 256;                            /* subtract 256 to convert to dBm */
        val = abs(CLAMP(val, -100, -40) + 40); /* normalize to 0 */
        val = 100 - (int) ((100.0 * (double) val) / 60.0);
    } else {
        /* Assume signal is a "quality" percentage */
    }

    return CLAMP(val, 0, 100);
}