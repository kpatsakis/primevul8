rb_reg_init_str_enc(VALUE re, VALUE s, rb_encoding *enc, int options)
{
    onig_errmsg_buffer err = "";

    if (rb_reg_initialize(re, RSTRING_PTR(s), RSTRING_LEN(s),
			  enc, options, err, NULL, 0) != 0) {
	rb_reg_raise_str(s, options, err);
    }
    reg_set_source(re, s, enc);

    return re;
}