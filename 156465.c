	std::string bdecode_node::dict_find_string_value(char const* key
		, char const* default_value) const
	{
		bdecode_node n = dict_find(key);
		if (n.type() != bdecode_node::string_t) return default_value;
		return n.string_value();
	}