TORRENT_TEST(unepected_eof2)
{
	char b[] = "l2:..0"; // expected ':' delimiter instead of EOF

	bdecode_node e;
	error_code ec;
	int pos;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec, &pos);
	TEST_EQUAL(ret, -1);
	TEST_EQUAL(pos, 6);
	TEST_EQUAL(ec, error_code(bdecode_errors::unexpected_eof));
	printf("%s\n", print_entry(e).c_str());
}