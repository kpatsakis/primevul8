TORRENT_TEST(close_overflow_length_prefix)
{
	char b[] = "99999999:foobar";
	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 8);
	TEST_EQUAL(ec, error_code(bdecode_errors::unexpected_eof));
	printf("%s\n", print_entry(e).c_str());
}