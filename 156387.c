TORRENT_TEST(swap_disjoint)
{
	char b1[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	char b2[] = "li1e3:fooli1ei2eed1:xi1eee";

	bdecode_node e1_root;
	bdecode_node e2_root;

	error_code ec;

	int ret = bdecode(b1, b1 + sizeof(b1)-1, e1_root, ec);
	TEST_EQUAL(ret, 0);
	ret = bdecode(b2, b2 + sizeof(b2)-1, e2_root, ec);
	TEST_EQUAL(ret, 0);

	bdecode_node e1 = e1_root.dict_find("c").list_at(0);
	bdecode_node e2 = e2_root.list_at(1);

	std::string str1 = print_entry(e1);
	std::string str2 = print_entry(e2);
	TEST_EQUAL(e1.type(), bdecode_node::int_t);
	TEST_EQUAL(e2.type(), bdecode_node::string_t);

	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::string_t);
	TEST_EQUAL(e2.type(), bdecode_node::int_t);
	TEST_EQUAL(print_entry(e1), str2);
	TEST_EQUAL(print_entry(e2), str1);

	// swap back
	e1.swap(e2);

	TEST_EQUAL(e1.type(), bdecode_node::int_t);
	TEST_EQUAL(e2.type(), bdecode_node::string_t);
	TEST_EQUAL(print_entry(e1), str1);
	TEST_EQUAL(print_entry(e2), str2);
}