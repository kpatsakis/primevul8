TORRENT_TEST(dict)
{
	char b[] = "d1:ai12453e1:b3:aaa1:c3:bbb1:X10:0123456789e";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());
	std::pair<const char*, int> section = e.data_section();
	TEST_CHECK(std::memcmp(b, section.first, section.second) == 0);
	TEST_CHECK(section.second == sizeof(b) - 1);
	TEST_CHECK(e.type() == bdecode_node::dict_t);
	TEST_CHECK(e.dict_size() == 4);
	TEST_CHECK(e.dict_find("a").type() == bdecode_node::int_t);
	TEST_CHECK(e.dict_find("a").int_value() == 12453);
	TEST_CHECK(e.dict_find("b").type() == bdecode_node::string_t);
	TEST_CHECK(e.dict_find("b").string_value() == std::string("aaa"));
	TEST_CHECK(e.dict_find("b").string_length() == 3);
	TEST_CHECK(e.dict_find("c").type() == bdecode_node::string_t);
	TEST_CHECK(e.dict_find("c").string_value() == std::string("bbb"));
	TEST_CHECK(e.dict_find("c").string_length() == 3);
	TEST_CHECK(e.dict_find_string_value("X") == "0123456789");
}