xps_parse_glyph_metrics(char *s, float *advance, float *uofs, float *vofs, int bidi_level)
{
    s = xps_parse_glyph_advance(s, advance, bidi_level);
    s = xps_parse_glyph_offsets(s, uofs, vofs);
    return s;
}