check_charset(char *p)
{
    return wc_guess_charset(p, 0) ? p : NULL;
}