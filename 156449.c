	int bdecode_node::list_size() const
	{
		TORRENT_ASSERT(type() == list_t);

		if (m_size != -1) return m_size;

		// make sure this is a list.
		bdecode_token const* tokens = m_root_tokens;
		TORRENT_ASSERT(tokens[m_token_idx].type == bdecode_token::list);

		// this is the first item
		int token = m_token_idx + 1;
		int ret = 0;
		
		// do we have a lookup cached?
		if (m_last_index != -1)
		{
			token = m_last_token;
			ret = m_last_index;
		}
		while (tokens[token].type != bdecode_token::end)
		{
			token += tokens[token].next_item;
			++ret;
		}

		m_size = ret;

		return ret;
	}