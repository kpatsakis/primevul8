TORRENT_TEST(list)
{
	char b[] = "li12453e3:aaae";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_CHECK(ret == 0);
	printf("%s\n", print_entry(e).c_str());
	std::pair<const char*, int> section = e.data_section();
	TEST_CHECK(std::memcmp(b, section.first, section.second) == 0);
	TEST_CHECK(section.second == sizeof(b) - 1);
	TEST_CHECK(e.type() == bdecode_node::list_t);
	TEST_CHECK(e.list_size() == 2);
	TEST_CHECK(e.list_at(0).type() == bdecode_node::int_t);
	TEST_CHECK(e.list_at(1).type() == bdecode_node::string_t);
	TEST_CHECK(e.list_at(0).int_value() == 12453);
	TEST_CHECK(e.list_at(1).string_value() == std::string("aaa"));
	TEST_CHECK(e.list_at(1).string_length() == 3);
	section = e.list_at(1).data_section();
	TEST_CHECK(std::memcmp("3:aaa", section.first, section.second) == 0);
	TEST_CHECK(section.second == 5);
}