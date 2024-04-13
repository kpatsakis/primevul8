TORRENT_TEST(invalid_digit)
{
	char b[] = "0o";
	boost::int64_t val = 0;
	bdecode_errors::error_code_enum ec;
	char const* e = parse_int(b, b + sizeof(b)-1, 'e', val, ec);
	TEST_EQUAL(ec, bdecode_errors::expected_digit);
	TEST_EQUAL(e, b + 1);
}