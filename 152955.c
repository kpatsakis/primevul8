Ta3Parser_ParseStringFlagsFilename(const char *s, const char *filename,
                          grammar *g, int start,
                          perrdetail *err_ret, int flags)
{
    int iflags = flags;
    return Ta3Parser_ParseStringFlagsFilenameEx(s, filename, g, start,
                                               err_ret, &iflags);
}