TORRENT_TEST(string)
{
	char b[] = "26:abcdefghijklmnopqrstuvwxyz";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_CHECK(ret == 0);
	printf("%s\n", print_entry(e).c_str());
	std::pair<const char*, int> section = e.data_section();
	TEST_CHECK(std::memcmp(b, section.first, section.second) == 0);
	TEST_EQUAL(section.second, sizeof(b) - 1);
	TEST_EQUAL(e.type(), bdecode_node::string_t);
	TEST_EQUAL(e.string_value(), std::string("abcdefghijklmnopqrstuvwxyz"));
	TEST_EQUAL(e.string_length(), 26);
}