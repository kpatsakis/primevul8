TORRENT_TEST(expected_string_dict_key2)
{
	char b[] = "df00:";

	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 1);
	TEST_EQUAL(ec, error_code(bdecode_errors::expected_digit));
	printf("%s\n", print_entry(e).c_str());
}