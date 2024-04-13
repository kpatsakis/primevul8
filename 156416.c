	bdecode_node bdecode_node::dict_find_string(char const* key) const
	{
		bdecode_node ret = dict_find(key);
		if (ret.type() == bdecode_node::string_t)
			return ret;
		return bdecode_node();
	}