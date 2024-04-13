rb_reg_init_str(VALUE re, VALUE s, int options)
{
    onig_errmsg_buffer err = "";

    if (rb_reg_initialize_str(re, s, options, err, NULL, 0) != 0) {
	rb_reg_raise_str(s, options, err);
    }

    return re;
}