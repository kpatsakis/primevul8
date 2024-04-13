TORRENT_TEST(swap_root_disjoint)
{
	char b1[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	char b2[] = "li1e3:fooli1ei2eed1:xi1eee";

	bdecode_node e1_root;
	bdecode_node e2;

	error_code ec;

	int ret = bdecode(b1, b1 + sizeof(b1)-1, e1_root, ec);
	TEST_EQUAL(ret, 0);
	ret = bdecode(b2, b2 + sizeof(b2)-1, e2, ec);
	TEST_EQUAL(ret, 0);

	bdecode_node e1 = e1_root.dict_find("d");

	std::string str1 = print_entry(e1);
	std::string str2 = print_entry(e2);
	TEST_EQUAL(e1.type(), bdecode_node::dict_t);
	TEST_EQUAL(e2.type(), bdecode_node::list_t);

	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::list_t);
	TEST_EQUAL(e2.type(), bdecode_node::dict_t);
	TEST_EQUAL(print_entry(e1), str2);
	TEST_EQUAL(print_entry(e2), str1);

	// swap back
	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::dict_t);
	TEST_EQUAL(e2.type(), bdecode_node::list_t);
	TEST_EQUAL(print_entry(e1), str1);
	TEST_EQUAL(print_entry(e2), str2);
}