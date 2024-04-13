TORRENT_TEST(parse_length_overflow)
{
	char const* b[] = {
		"d1:a1919191010:11111",
		"d2143289344:a4:aaaae",
		"d214328934114:a4:aaaae",
		"d9205357638345293824:a4:aaaae",
		"d1:a9205357638345293824:11111",
	};

	for (int i = 0; i < int(sizeof(b)/sizeof(b[0])); ++i)
	{
		error_code ec;
		bdecode_node e;
		int ret = bdecode(b[i], b[i] + strlen(b[i]), e, ec);
		TEST_EQUAL(ret, -1);
		TEST_CHECK(ec == error_code(bdecode_errors::unexpected_eof));
	}
}