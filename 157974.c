PyParser_SimpleParseFile(FILE *fp, const char *filename, int start)
{
    return PyParser_SimpleParseFileFlags(fp, filename, start, 0);
}