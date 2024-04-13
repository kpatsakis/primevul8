match_except(const char *spec_str, gboolean *out_except)
{
    if (_MATCH_CHECK(spec_str, EXCEPT_TAG))
        *out_except = TRUE;
    else
        *out_except = FALSE;
    return spec_str;
}