rb_backref_set_string(VALUE string, long pos, long len)
{
    VALUE match = rb_backref_get();
    if (NIL_P(match) || FL_TEST(match, MATCH_BUSY)) {
	match = match_alloc(rb_cMatch);
    }
    match_set_string(match, string, pos, len);
    rb_backref_set(match);
}