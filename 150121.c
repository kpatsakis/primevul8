unescape_unicode_bmp(const char **pp, const char *end,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    const char *p = *pp;
    size_t len;
    unsigned long code;

    if (end < p+4) {
        errcpy(err, "invalid Unicode escape");
        return -1;
    }
    code = ruby_scan_hex(p, 4, &len);
    if (len != 4) {
        errcpy(err, "invalid Unicode escape");
        return -1;
    }
    if (append_utf8(code, buf, encp, err) != 0)
        return -1;
    *pp = p + 4;
    return 0;
}