PyParser_ParseFile(FILE *fp, const char *filename, grammar *g, int start,
                   const char *ps1, const char *ps2,
                   perrdetail *err_ret)
{
    return PyParser_ParseFileFlags(fp, filename, NULL,
                                   g, start, ps1, ps2, err_ret, 0);
}