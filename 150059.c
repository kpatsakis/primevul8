rb_reg_preprocess(const char *p, const char *end, rb_encoding *enc,
        rb_encoding **fixed_enc, onig_errmsg_buffer err)
{
    VALUE buf;
    int has_property = 0;

    buf = rb_str_buf_new(0);

    if (rb_enc_asciicompat(enc))
        *fixed_enc = 0;
    else {
        *fixed_enc = enc;
        rb_enc_associate(buf, enc);
    }

    if (unescape_nonascii(p, end, enc, buf, fixed_enc, &has_property, err) != 0)
        return Qnil;

    if (has_property && !*fixed_enc) {
        *fixed_enc = enc;
    }

    if (*fixed_enc) {
        rb_enc_associate(buf, *fixed_enc);
    }

    return buf;
}