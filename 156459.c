TORRENT_TEST(print_entry2)
{
	char b[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(print_entry(e), "{ 'a': 1, 'b': 'foo', 'c': [ 1, 2 ], 'd': { 'x': 1 } }");
}