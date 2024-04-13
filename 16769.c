number_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    return p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}