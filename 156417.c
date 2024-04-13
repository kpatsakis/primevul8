TORRENT_TEST(dict_null_key)
{
	char b[] = "d3:a\0bi1ee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_CHECK(ret == 0);
	TEST_CHECK(e.dict_size() == 1);
	bdecode_node d = e.dict_find(std::string("a\0b", 3));
	TEST_EQUAL(d.type(), bdecode_node::int_t);
	TEST_EQUAL(d.int_value(), 1);
}