TORRENT_TEST(integer)
{
	char b[] = "i12453e";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_CHECK(ret == 0);
	printf("%s\n", print_entry(e).c_str());
	std::pair<const char*, int> section = e.data_section();
	TEST_CHECK(std::memcmp(b, section.first, section.second) == 0);
	TEST_CHECK(section.second == sizeof(b) - 1);
	TEST_CHECK(e.type() == bdecode_node::int_t);
	TEST_CHECK(e.int_value() == 12453);
}