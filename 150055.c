check_unicode_range(unsigned long code, onig_errmsg_buffer err)
{
    if ((0xd800 <= code && code <= 0xdfff) || /* Surrogates */
        0x10ffff < code) {
        errcpy(err, "invalid Unicode range");
        return -1;
    }
    return 0;
}