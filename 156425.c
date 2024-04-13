TORRENT_TEST(clear)
{
	char b1[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	char b2[] = "li1ei2ee";

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b1, b1 + sizeof(b1)-1, e, ec);
	printf("%s\n", print_entry(e).c_str());
	TEST_EQUAL(ret, 0);
	TEST_EQUAL(e.type(), bdecode_node::dict_t);
	TEST_EQUAL(e.dict_size(), 4);
	TEST_EQUAL(e.dict_at(1).first, "b");

	ret = bdecode(b2, b2 + sizeof(b2)-1, e, ec);
	printf("%s\n", print_entry(e).c_str());
	TEST_EQUAL(ret, 0);
	TEST_EQUAL(e.type(), bdecode_node::list_t);
	TEST_EQUAL(e.list_size(), 2);
	TEST_EQUAL(e.list_int_value_at(1), 2);
}