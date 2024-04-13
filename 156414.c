TORRENT_TEST(parse_int)
{
	char b[] = "1234567890e";
	boost::int64_t val = 0;
	bdecode_errors::error_code_enum ec;
	char const* e = parse_int(b, b + sizeof(b)-1, 'e', val, ec);
	TEST_EQUAL(val, 1234567890);
	TEST_EQUAL(e, b + sizeof(b) - 2);
}