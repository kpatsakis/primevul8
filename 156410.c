TORRENT_TEST(dict_ints)
{
	std::string buf;
	buf += "d";
	for (int i = 0; i < 1000; ++i)
	{
		char tmp[30];
		snprintf(tmp, sizeof(tmp), "4:%04di%de", i, i);
		buf += tmp;
	}
	buf += "e";

	printf("%s\n", buf.c_str());
	bdecode_node e;
	error_code ec;
	int ret = bdecode((char*)&buf[0], (char*)&buf[0] + buf.size(), e, ec);
	TEST_EQUAL(ret, 0);
	TEST_EQUAL(e.type(), bdecode_node::dict_t);
	TEST_EQUAL(e.dict_size(), 1000);
	for (int i = 0; i < 1000; ++i)
	{
		char tmp[30];
		snprintf(tmp, sizeof(tmp), "%04d", i);
		TEST_EQUAL(e.dict_find_int_value(tmp), i);
	}
}