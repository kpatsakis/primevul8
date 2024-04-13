static char *read_file_recode(ASS_Library *library, char *fname,
                              char *codepage, size_t *size)
{
    char *buf;
    size_t bufsize;

    buf = read_file(library, fname, &bufsize);
    if (!buf)
        return 0;
#ifdef CONFIG_ICONV
    if (codepage) {
        char *tmpbuf = sub_recode(library, buf, bufsize, codepage);
        free(buf);
        buf = tmpbuf;
    }
    if (!buf)
        return 0;
#endif
    *size = bufsize;
    return buf;
}