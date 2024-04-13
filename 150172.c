option_to_str(char str[4], int options)
{
    char *p = str;
    if (options & ONIG_OPTION_MULTILINE) *p++ = 'm';
    if (options & ONIG_OPTION_IGNORECASE) *p++ = 'i';
    if (options & ONIG_OPTION_EXTEND) *p++ = 'x';
    *p = 0;
    return str;
}