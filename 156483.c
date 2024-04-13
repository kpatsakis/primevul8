	std::string bdecode_error_category::message(int ev) const BOOST_SYSTEM_NOEXCEPT
	{
		static char const* msgs[] =
		{
			"no error",
			"expected digit in bencoded string",
			"expected colon in bencoded string",
			"unexpected end of file in bencoded string",
			"expected value (list, dict, int or string) in bencoded string",
			"bencoded nesting depth exceeded",
			"bencoded item count limit exceeded",
			"integer overflow",
		};
		if (ev < 0 || ev >= int(sizeof(msgs)/sizeof(msgs[0])))
			return "Unknown error";
		return msgs[ev];
	}