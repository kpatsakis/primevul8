char *read_file(ASS_Library *library, char *fname, size_t *bufsize)
{
    int res;
    long sz;
    long bytes_read;
    char *buf;

    FILE *fp = fopen(fname, "rb");
    if (!fp) {
        ass_msg(library, MSGL_WARN,
                "ass_read_file(%s): fopen failed", fname);
        return 0;
    }
    res = fseek(fp, 0, SEEK_END);
    if (res == -1) {
        ass_msg(library, MSGL_WARN,
                "ass_read_file(%s): fseek failed", fname);
        fclose(fp);
        return 0;
    }

    sz = ftell(fp);
    rewind(fp);

    ass_msg(library, MSGL_V, "File size: %ld", sz);

    buf = sz < SIZE_MAX ? malloc(sz + 1) : NULL;
    if (!buf) {
        fclose(fp);
        return NULL;
    }
    assert(buf);
    bytes_read = 0;
    do {
        res = fread(buf + bytes_read, 1, sz - bytes_read, fp);
        if (res <= 0) {
            ass_msg(library, MSGL_INFO, "Read failed, %d: %s", errno,
                    strerror(errno));
            fclose(fp);
            free(buf);
            return 0;
        }
        bytes_read += res;
    } while (sz - bytes_read > 0);
    buf[sz] = '\0';
    fclose(fp);

    if (bufsize)
        *bufsize = sz;
    return buf;
}