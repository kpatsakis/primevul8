	int bdecode_node::string_length() const
	{
		TORRENT_ASSERT(type() == string_t);
		bdecode_token const& t = m_root_tokens[m_token_idx];
		TORRENT_ASSERT(t.type == bdecode_token::string);
		return m_root_tokens[m_token_idx + 1].offset - t.offset - t.start_offset();
	}