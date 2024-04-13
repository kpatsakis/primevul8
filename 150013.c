append_utf8(unsigned long uv,
        VALUE buf, rb_encoding **encp, onig_errmsg_buffer err)
{
    if (check_unicode_range(uv, err) != 0)
        return -1;
    if (uv < 0x80) {
        char escbuf[5];
        snprintf(escbuf, sizeof(escbuf), "\\x%02X", (int)uv);
        rb_str_buf_cat(buf, escbuf, 4);
    }
    else {
        int len;
        char utf8buf[6];
        len = rb_uv_to_utf8(utf8buf, uv);
        rb_str_buf_cat(buf, utf8buf, len);

        if (*encp == 0)
            *encp = rb_utf8_encoding();
        else if (*encp != rb_utf8_encoding()) {
            errcpy(err, "UTF-8 character in non UTF-8 regexp");
            return -1;
        }
    }
    return 0;
}