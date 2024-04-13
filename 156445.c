	boost::int64_t bdecode_node::list_int_value_at(int i
		, boost::int64_t default_val)
	{
		bdecode_node n = list_at(i);
		if (n.type() != bdecode_node::int_t) return default_val;
		return n.int_value();
	}