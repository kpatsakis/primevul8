TORRENT_TEST(exceed_buf_limit)
{
	char b[] = "l3:fooe";

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + 0x3fffffff, e, ec);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(ec, error_code(bdecode_errors::limit_exceeded));
	printf("%s\n", print_entry(e).c_str());
}