TORRENT_TEST(negative_length_prefix)
{
	char b[] = "-10:foobar";
	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 0);
	TEST_EQUAL(ec, error_code(bdecode_errors::expected_value));
	printf("%s\n", print_entry(e).c_str());
}