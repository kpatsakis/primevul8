xps_parse_real_num(char *s, float *number, bool *number_parsed)
{
    char *tail;
    float v;
    v = (float)strtod(s, &tail);
    *number_parsed = tail != s;
    if (*number_parsed)
        *number = v;
    return tail;
}