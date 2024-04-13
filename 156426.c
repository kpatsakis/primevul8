	bdecode_node bdecode_node::dict_find_list(char const* key) const
	{
		bdecode_node ret = dict_find(key);
		if (ret.type() == bdecode_node::list_t)
			return ret;
		return bdecode_node();
	}