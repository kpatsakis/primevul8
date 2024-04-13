TORRENT_TEST(dict_find_funs)
{
	// a: int
	// b: string
	// c: list
	// d: dict
	char b[] = "d1:ai1e1:b3:foo1:cli1ei2ee1:dd1:xi1eee";
	bdecode_node e;
	error_code ec;
	int ret = bdecode(b, b + sizeof(b)-1, e, ec);
	TEST_EQUAL(ret, 0);
	printf("%s\n", print_entry(e).c_str());

	TEST_EQUAL(e.type(), bdecode_node::dict_t);

	// dict_find_int* 

	TEST_EQUAL(e.dict_find_int_value("a"), 1);
	TEST_EQUAL(e.dict_find_int("a").type(), bdecode_node::int_t);
	TEST_EQUAL(e.dict_find_int_value("b", -10), -10);
	TEST_EQUAL(e.dict_find_int_value("x", -10), -10);
	TEST_EQUAL(e.dict_find_int("b").type(), bdecode_node::none_t);
	TEST_EQUAL(e.dict_find_int("x").type(), bdecode_node::none_t);

	// dict_find_string*

	TEST_EQUAL(e.dict_find_string_value("b"), "foo");
	TEST_EQUAL(e.dict_find_string("b").type(), bdecode_node::string_t);
	TEST_EQUAL(e.dict_find_string_value("c", "blah"), "blah");
	TEST_EQUAL(e.dict_find_string_value("x", "blah"), "blah");
	TEST_EQUAL(e.dict_find_string("c").type(), bdecode_node::none_t);
	TEST_EQUAL(e.dict_find_string("x").type(), bdecode_node::none_t);

	// dict_find_list

	TEST_CHECK(e.dict_find_list("c"));
	TEST_EQUAL(e.dict_find_list("c").list_size(), 2);
	TEST_EQUAL(e.dict_find_list("c").list_int_value_at(0), 1);
	TEST_EQUAL(e.dict_find_list("c").list_int_value_at(1), 2);
	TEST_CHECK(!e.dict_find_list("d"));

	// dict_find_dict

	TEST_CHECK(e.dict_find_dict("d"));
	TEST_EQUAL(e.dict_find_dict("d").dict_find_int_value("x"), 1);
	TEST_EQUAL(e.dict_find_dict("d").dict_find_int_value("y", -10), -10);
	TEST_CHECK(!e.dict_find_dict("c"));

	// variants taking std::string
	TEST_EQUAL(e.dict_find_dict(std::string("d")).dict_find_int_value("x"), 1);
	TEST_CHECK(!e.dict_find_dict(std::string("c")));
	TEST_CHECK(!e.dict_find_dict(std::string("x")));

	TEST_EQUAL(e.dict_size(), 4);
	TEST_EQUAL(e.dict_size(), 4);

	// dict_at

	TEST_EQUAL(e.dict_at(0).first, "a");
	TEST_EQUAL(e.dict_at(0).second.int_value(), 1);
	TEST_EQUAL(e.dict_at(1).first, "b");
	TEST_EQUAL(e.dict_at(1).second.string_value(), "foo");
	TEST_EQUAL(e.dict_at(2).first, "c");
	TEST_EQUAL(e.dict_at(2).second.type(), bdecode_node::list_t);
	TEST_EQUAL(e.dict_at(3).first, "d");
	TEST_EQUAL(e.dict_at(3).second.type(), bdecode_node::dict_t);
}