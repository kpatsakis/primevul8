    caca_get_dither_color_list(caca_dither_t const *d)
{
    static char const * const list[] =
    {
        "mono", "white on black",
        "gray", "grayscale on black",
        "8", "8 colours on black",
        "16", "16 colours on black",
        "fullgray", "full grayscale",
        "full8", "full 8 colours",
        "full16", "full 16 colours",
        NULL, NULL
    };

    return list;
}