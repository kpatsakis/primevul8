rb_reg_search(VALUE re, VALUE str, long pos, int reverse)
{
    return rb_reg_search0(re, str, pos, reverse, 1);
}