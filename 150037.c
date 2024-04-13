match_check(VALUE match)
{
    if (!RMATCH(match)->regexp) {
	rb_raise(rb_eTypeError, "uninitialized Match");
    }
}