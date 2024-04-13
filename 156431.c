	std::string bdecode_node::list_string_value_at(int i
		, char const* default_val)
	{
		bdecode_node n = list_at(i);
		if (n.type() != bdecode_node::string_t) return default_val;
		return n.string_value();
	}