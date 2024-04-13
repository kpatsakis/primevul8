    caca_get_dither_antialias_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "none", "No antialiasing",
        "prefilter", "Prefilter antialiasing",
        NULL, NULL
    };

    return list;
}