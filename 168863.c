xps_parse_glyph_advance(char *s, float *advance, int bidi_level)
{
    bool advance_overridden = false;
    
    if (*s == ',') {
        s = xps_parse_real_num(s + 1, advance, &advance_overridden);

        /*
         * If the advance has been derived from the font and not
         * overridden by the Indices Attribute the sign has already
         * been direction adjusted.
         */

        if (advance_overridden && (bidi_level & 1))
            *advance *= -1;
    }
    return s;
}