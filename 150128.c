rb_reg_check_preprocess(VALUE str)
{
    rb_encoding *fixed_enc = 0;
    onig_errmsg_buffer err = "";
    VALUE buf;
    char *p, *end;
    rb_encoding *enc;

    StringValue(str);
    p = RSTRING_PTR(str);
    end = p + RSTRING_LEN(str);
    enc = rb_enc_get(str);

    buf = rb_reg_preprocess(p, end, enc, &fixed_enc, err);
    RB_GC_GUARD(str);

    if (buf == Qnil) {
	return rb_reg_error_desc(str, 0, err);
    }
    return Qnil;
}