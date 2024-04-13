PyParser_SimpleParseString(const char *str, int start)
{
    return PyParser_SimpleParseStringFlags(str, start, 0);
}