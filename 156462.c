	std::pair<std::string, bdecode_node> bdecode_node::dict_at(int i) const
	{
		TORRENT_ASSERT(type() == dict_t);
		TORRENT_ASSERT(m_token_idx != -1);
	
		bdecode_token const* tokens = m_root_tokens;
		TORRENT_ASSERT(tokens[m_token_idx].type == bdecode_token::dict);

		int token = m_token_idx + 1;
		int item = 0;

		// do we have a lookup cached?
		if (m_last_index <= i && m_last_index != -1)
		{
			token = m_last_token;
			item = m_last_index;
		}

		while (item < i)
		{
			TORRENT_ASSERT(tokens[token].type == bdecode_token::string);

			// skip the key
			token += tokens[token].next_item;
			TORRENT_ASSERT(tokens[token].type != bdecode_token::end);

			// skip the value
			token += tokens[token].next_item;

			++item;

			// index 'i' out of range
			TORRENT_ASSERT(tokens[token].type != bdecode_token::end);
		}

		// there's no point in caching the first item
		if (i > 0)
		{
			m_last_token = token;
			m_last_index = i;
		}

		int value_token = token + tokens[token].next_item;
		TORRENT_ASSERT(tokens[token].type != bdecode_token::end);

		return std::make_pair(
			bdecode_node(tokens, m_buffer, m_buffer_size, token).string_value()
			, bdecode_node(tokens, m_buffer, m_buffer_size, value_token));
	}