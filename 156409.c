TORRENT_TEST(int_overflow2)
{
	char b[] = "i184467440737095516154e";
	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 22);
	TEST_EQUAL(ec,  error_code(bdecode_errors::overflow));
	printf("%s\n", print_entry(e).c_str());
}