	int bdecode_node::dict_size() const
	{
		TORRENT_ASSERT(type() == dict_t);
		TORRENT_ASSERT(m_token_idx != -1);

		if (m_size != -1) return m_size;

		bdecode_token const* tokens = m_root_tokens;
		TORRENT_ASSERT(tokens[m_token_idx].type == bdecode_token::dict);

		// this is the first item
		int token = m_token_idx + 1;
		int ret = 0;

		if (m_last_index != -1)
		{
			ret = m_last_index * 2;
			token = m_last_token;
		}

		while (tokens[token].type != bdecode_token::end)
		{
			token += tokens[token].next_item;
			++ret;
		}

		// a dictionary must contain full key-value pairs. which means
		// the number of entries is divisible by 2
		TORRENT_ASSERT((ret % 2) == 0);

		// each item is one key and one value, so divide by 2
		ret /= 2;

		m_size = ret;

		return ret;
	}