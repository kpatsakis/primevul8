TORRENT_TEST(parse_int_overflow)
{
	char b[] = "9223372036854775808:";
	boost::int64_t val = 0;
	bdecode_errors::error_code_enum ec;
	char const* e = parse_int(b, b + sizeof(b)-1, ':', val, ec);
	TEST_EQUAL(ec, bdecode_errors::overflow);
	TEST_EQUAL(e, b + 18);
}