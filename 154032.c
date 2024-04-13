maximum_visible_length_plain(char *str, int offset)
{
    visible_length_offset = offset;
    return visible_length_plain(str);
}