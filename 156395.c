TORRENT_TEST(switch_buffer)
{
	char b1[] = "d1:ai1e1:b3:foo1:cli1e-i2ee1:dd1:xi1eee";
	char b2[] = "d1:ai1e1:b3:foo1:cli1e-i2ee1:dd1:xi1eee";

	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b1, b1 + sizeof(b1)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 22);
	TEST_EQUAL(e.type(), bdecode_node::dict_t);

	std::string string1 = print_entry(e);
	printf("%s\n", string1.c_str());

	e.switch_underlying_buffer(b2);

	std::string string2 = print_entry(e);
	printf("%s\n", string2.c_str());

	TEST_EQUAL(string1, string2);
}