TORRENT_TEST(64bit_int_negative)
{
	char b[] = "i-9223372036854775807e";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_CHECK(ret == 0);
	printf("%s\n", print_entry(e).c_str());
	TEST_CHECK(e.int_value() == -9223372036854775807LL);
}