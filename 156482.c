TORRENT_TEST(partial_parse2)
{
	char b[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:d-d1:xi1eee";

	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 29);
	TEST_EQUAL(e.type(), bdecode_node::dict_t);

	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(print_entry(e), "{ 'a': 1, 'b': 'foo', 'c': [ 1, 2 ], 'd': {} }");
}