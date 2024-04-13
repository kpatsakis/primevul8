Ta3Parser_ParseFileFlags(FILE *fp, const char *filename, const char *enc,
                        grammar *g, int start,
                        const char *ps1, const char *ps2,
                        perrdetail *err_ret, int flags)
{
    int iflags = flags;
    return Ta3Parser_ParseFileFlagsEx(fp, filename, enc, g, start, ps1,
                                     ps2, err_ret, &iflags);
}