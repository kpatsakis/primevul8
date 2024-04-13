TORRENT_TEST(premature_e)
{
	char b[] = "e";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(ec, error_code(bdecode_errors::unexpected_eof));
	printf("%s\n", print_entry(e).c_str());
}