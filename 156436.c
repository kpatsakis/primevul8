TORRENT_TEST(depth_limit)
{
	char b[2048];
	for (int i = 0; i < 1024; ++i)
		b[i]= 'l';

	for (int i = 1024; i < 2048; ++i)
		b[i]= 'e';

	// 1024 levels nested lists

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b), e, ec, NULL, 100);
	TEST_CHECK(ret != 0);
	TEST_EQUAL(ec, error_code(bdecode_errors::depth_exceeded
		, get_bdecode_category()));
}