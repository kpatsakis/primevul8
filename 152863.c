Ta3Parser_ParseStringFlags(const char *s, grammar *g, int start,
                          perrdetail *err_ret, int flags)
{
    return Ta3Parser_ParseStringFlagsFilename(s, NULL,
                                             g, start, err_ret, flags);
}