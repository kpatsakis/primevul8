PyParser_SimpleParseFileFlags(FILE *fp, const char *filename, int start, int flags)
{
    perrdetail err;
    node *n = PyParser_ParseFileFlags(fp, filename, NULL,
                                      &_PyParser_Grammar,
                                      start, NULL, NULL, &err, flags);
    if (n == NULL)
        err_input(&err);
    err_free(&err);

    return n;
}