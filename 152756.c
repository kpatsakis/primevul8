Ta3Parser_ParseString(const char *s, grammar *g, int start, perrdetail *err_ret)
{
    return Ta3Parser_ParseStringFlagsFilename(s, NULL, g, start, err_ret, 0);
}