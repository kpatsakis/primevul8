	bdecode_node bdecode_node::dict_find(std::string key) const
	{
		TORRENT_ASSERT(type() == dict_t);

		bdecode_token const* tokens = m_root_tokens;
	
		// this is the first item
		int token = m_token_idx + 1;

		while (tokens[token].type != bdecode_token::end)
		{
			bdecode_token const& t = tokens[token];
			TORRENT_ASSERT(t.type == bdecode_token::string);
			int size = m_root_tokens[token + 1].offset - t.offset - t.start_offset();
			if (int(key.size()) == size
				&& std::equal(key.c_str(), key.c_str() + size, m_buffer
					+ t.offset + t.start_offset()))
			{
				// skip key
				token += t.next_item;
				TORRENT_ASSERT(tokens[token].type != bdecode_token::end);
			
				return bdecode_node(tokens, m_buffer, m_buffer_size, token);
			}

			// skip key
			token += t.next_item;
			TORRENT_ASSERT(tokens[token].type != bdecode_token::end);

			// skip value
			token += tokens[token].next_item;
		}

		return bdecode_node();
	}