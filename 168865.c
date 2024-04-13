xps_parse_glyph_offsets(char *s, float *uofs, float *vofs)
{
    bool offsets_overridden; /* not used */

    if (*s == ',')
        s = xps_parse_real_num(s + 1, uofs, &offsets_overridden);
    if (*s == ',')
        s = xps_parse_real_num(s + 1, vofs, &offsets_overridden);
    return s;
}