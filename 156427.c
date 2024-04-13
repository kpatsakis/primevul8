TORRENT_TEST(dict_at)
{
	char b[] = "d3:fooi1e3:bari2ee";

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);

	TEST_EQUAL(e.type(), bdecode_node::dict_t);
	TEST_EQUAL(e.dict_size(), 2);
	TEST_EQUAL(e.dict_at(0).first, "foo");
	TEST_EQUAL(e.dict_at(0).second.type(), bdecode_node::int_t);
	TEST_EQUAL(e.dict_at(0).second.int_value(), 1);
	TEST_EQUAL(e.dict_at(1).first, "bar");
	TEST_EQUAL(e.dict_at(1).second.type(), bdecode_node::int_t);
	TEST_EQUAL(e.dict_at(1).second.int_value(), 2);
}