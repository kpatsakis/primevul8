PyTokenizer_FindEncoding(int fd)
{
    return PyTokenizer_FindEncodingFilename(fd, NULL);
}