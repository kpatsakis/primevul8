	bdecode_node bdecode_node::dict_find_dict(char const* key) const
	{
		bdecode_node ret = dict_find(key);
		if (ret.type() == bdecode_node::dict_t)
			return ret;
		return bdecode_node();
	}