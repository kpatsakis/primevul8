TORRENT_TEST(print_entry)
{
	char b[] = "li1e3:fooli1ei2eed1:xi1eee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(print_entry(e), "[ 1, 'foo', [ 1, 2 ], { 'x': 1 } ]");
}