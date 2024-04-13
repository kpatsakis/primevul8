	boost::int64_t bdecode_node::dict_find_int_value(char const* key
		, boost::int64_t default_val) const
	{
		bdecode_node n = dict_find(key);
		if (n.type() != bdecode_node::int_t) return default_val;
		return n.int_value();
	}