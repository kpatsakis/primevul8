TORRENT_TEST(overflow_length_prefix2)
{
	char b[] = "199999999:foobar";
	bdecode_node e;
	error_code ec;
	int pos;
	// pretend that we have a large buffer like that
	int ret = bdecode(b, b + 999999999, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 0);
	TEST_EQUAL(ec, error_code(bdecode_errors::limit_exceeded));
	printf("%s\n", print_entry(e).c_str());
}