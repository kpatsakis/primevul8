TORRENT_TEST(list_at_reverse)
{
	// int
	// string
	// list
	// dict
	char b[] = "li1e3:fooli1ei2eed1:xi1eee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(e.type(), bdecode_node::list_t);

	TEST_EQUAL(e.list_at(3).type(), bdecode_node::dict_t);
	TEST_EQUAL(e.list_at(2).type(), bdecode_node::list_t);
	TEST_EQUAL(e.list_string_value_at(1), "foo");
	TEST_EQUAL(e.list_int_value_at(0), 1);

	TEST_EQUAL(e.list_size(), 4);
	TEST_EQUAL(e.list_size(), 4);
}