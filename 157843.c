PyParser_SimpleParseStringFlags(const char *str, int start, int flags)
{
    perrdetail err;
    node *n = PyParser_ParseStringFlags(str, &_PyParser_Grammar,
                                        start, &err, flags);
    if (n == NULL)
        err_input(&err);
    err_free(&err);
    return n;
}