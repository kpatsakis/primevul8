TORRENT_TEST(int_invalid_digit)
{
	char b[] = "i92337203t854775807e";
	bdecode_node e;
	error_code ec;
	int pos = 0;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 9);
	TEST_EQUAL(ec, error_code(bdecode_errors::expected_digit));
	printf("%s\n", print_entry(e).c_str());
}