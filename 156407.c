TORRENT_TEST(bdecode_error)
{
	error_code ec(bdecode_errors::overflow);
	TEST_EQUAL(ec.message(), "integer overflow");
	TEST_EQUAL(ec.category().name(), std::string("bdecode error"));
	ec.assign(5434, get_bdecode_category());
	TEST_EQUAL(ec.message(), "Unknown error");
}