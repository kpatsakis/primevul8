TORRENT_TEST(swap_root)
{
	char b1[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";

	bdecode_node e1;
	bdecode_node e2;

	error_code ec;

	int ret = bdecode(b1, b1 + sizeof(b1)-1, e1, ec);
	TEST_EQUAL(ret, 0);

	e2 = e1.dict_find("c").list_at(0);

	std::string str1 = print_entry(e1);
	std::string str2 = print_entry(e2);
	TEST_EQUAL(e1.type(), bdecode_node::dict_t);
	TEST_EQUAL(e2.type(), bdecode_node::int_t);
	printf("%s\n", print_entry(e1).c_str());

	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::int_t);
	TEST_EQUAL(e2.type(), bdecode_node::dict_t);
	TEST_EQUAL(print_entry(e1), str2);
	TEST_EQUAL(print_entry(e2), str1);
	printf("%s\n", print_entry(e1).c_str());

	// swap back
	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::dict_t);
	TEST_EQUAL(e2.type(), bdecode_node::int_t);
	TEST_EQUAL(print_entry(e1), str1);
	TEST_EQUAL(print_entry(e2), str2);
	printf("%s\n", print_entry(e1).c_str());
}