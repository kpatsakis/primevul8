static char *next_token(char **str)
{
    char *p;
    char *start;
    skip_spaces(str);
    if (**str == '\0') {
        return 0;
    }

    advance_token_pos((const char**)str,
                      (const char**)&start,
                      (const char**)&p);

    *p = '\0';
    return start;
}