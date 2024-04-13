TORRENT_TEST(item_limit)
{
	char b[10240];
	b[0] = 'l';
	int i = 1;
	for (i = 1; i < 10239; i += 2)
		memcpy(&b[i], "0:", 2);
	b[i] = 'e';

	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + i + 1, e, ec, NULL, 1000, 1000);
	TEST_CHECK(ret != 0);
	TEST_EQUAL(ec, error_code(bdecode_errors::limit_exceeded
		, get_bdecode_category()));
}