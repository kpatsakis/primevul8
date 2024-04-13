TORRENT_TEST(dict_find_funs2)
{
	// a: int
	// b: string
	// c: list
	// d: dict
	char b[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(e.type(), bdecode_node::dict_t);

	// try finding the last item in a dict (to skip all the other ones)
	TEST_EQUAL(e.dict_find("d").type(), bdecode_node::dict_t);
	TEST_EQUAL(e.dict_find(std::string("d")).type(), bdecode_node::dict_t);
}