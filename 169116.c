static char *sub_recode(ASS_Library *library, char *data, size_t size,
                        char *codepage)
{
    iconv_t icdsc;
    char *tocp = "UTF-8";
    char *outbuf;
    assert(codepage);

    if ((icdsc = iconv_open(tocp, codepage)) != (iconv_t) (-1)) {
        ass_msg(library, MSGL_V, "Opened iconv descriptor");
    } else {
        ass_msg(library, MSGL_ERR, "Error opening iconv descriptor");
        return NULL;
    }

    {
        size_t osize = size;
        size_t ileft = size;
        size_t oleft = size - 1;
        char *ip;
        char *op;
        size_t rc;
        int clear = 0;

        outbuf = malloc(osize);
        if (!outbuf)
            goto out;
        ip = data;
        op = outbuf;

        while (1) {
            if (ileft)
                rc = iconv(icdsc, &ip, &ileft, &op, &oleft);
            else {              // clear the conversion state and leave
                clear = 1;
                rc = iconv(icdsc, NULL, NULL, &op, &oleft);
            }
            if (rc == (size_t) (-1)) {
                if (errno == E2BIG) {
                    size_t offset = op - outbuf;
                    char *nbuf = realloc(outbuf, osize + size);
                    if (!nbuf) {
                        free(outbuf);
                        outbuf = 0;
                        goto out;
                    }
                    outbuf = nbuf;
                    op = outbuf + offset;
                    osize += size;
                    oleft += size;
                } else {
                    ass_msg(library, MSGL_WARN, "Error recoding file");
                    free(outbuf);
                    outbuf = NULL;
                    goto out;
                }
            } else if (clear)
                break;
        }
        outbuf[osize - oleft - 1] = 0;
    }

out:
    if (icdsc != (iconv_t) (-1)) {
        (void) iconv_close(icdsc);
        ass_msg(library, MSGL_V, "Closed iconv descriptor");
    }

    return outbuf;
}