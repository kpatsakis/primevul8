rb_reg_initialize_str(VALUE obj, VALUE str, int options, onig_errmsg_buffer err,
	const char *sourcefile, int sourceline)
{
    int ret;
    rb_encoding *str_enc = rb_enc_get(str), *enc = str_enc;
    if (options & ARG_ENCODING_NONE) {
        rb_encoding *ascii8bit = rb_ascii8bit_encoding();
        if (enc != ascii8bit) {
            if (str_coderange(str) != ENC_CODERANGE_7BIT) {
                errcpy(err, "/.../n has a non escaped non ASCII character in non ASCII-8BIT script");
                return -1;
            }
            enc = ascii8bit;
        }
    }
    ret = rb_reg_initialize(obj, RSTRING_PTR(str), RSTRING_LEN(str), enc,
			    options, err, sourcefile, sourceline);
    OBJ_INFECT(obj, str);
    if (ret == 0) reg_set_source(obj, str, str_enc);
    return ret;
}