TORRENT_TEST(string_ptr)
{
	char b[] = "l3:fooe";

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);

	TEST_EQUAL(e.type(), bdecode_node::list_t);
	TEST_EQUAL(e.list_size(), 1);
	TEST_EQUAL(e.list_at(0).type(), bdecode_node::string_t);
	TEST_EQUAL(e.list_at(0).string_ptr(), b + 3);
	TEST_EQUAL(e.list_at(0).string_length(), 3);
}