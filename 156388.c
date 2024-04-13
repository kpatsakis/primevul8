TORRENT_TEST(expected_colon_string)
{
	char b[] = "928";
	boost::int64_t val = 0;
	bdecode_errors::error_code_enum ec;
	char const* e = parse_int(b, b + sizeof(b)-1, ':', val, ec);
	TEST_EQUAL(ec, bdecode_errors::expected_colon);
	TEST_EQUAL(e, b + 3);
}