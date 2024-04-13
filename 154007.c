maximum_visible_length(char *str, int offset)
{
    visible_length_offset = offset;
    return visible_length(str);
}