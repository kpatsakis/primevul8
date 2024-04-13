TORRENT_TEST(list_ints)
{
	std::string buf;
	buf += "l";
	for (int i = 0; i < 1000; ++i)
	{
		char tmp[20];
		snprintf(tmp, sizeof(tmp), "i%de", i);
		buf += tmp;
	}
	buf += "e";

	bdecode_node e;
	error_code ec;
	int ret = bdecode((char*)&buf[0], (char*)&buf[0] + buf.size(), e, ec);
	TEST_EQUAL(ret, 0);
	TEST_EQUAL(e.type(), bdecode_node::list_t);
	TEST_EQUAL(e.list_size(), 1000);
	for (int i = 0; i < 1000; ++i)
	{
		TEST_EQUAL(e.list_int_value_at(i), i);
	}
}