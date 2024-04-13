	std::string bdecode_node::string_value() const
	{
		TORRENT_ASSERT(type() == string_t);
		bdecode_token const& t = m_root_tokens[m_token_idx];
		int size = m_root_tokens[m_token_idx + 1].offset - t.offset - t.start_offset();
		TORRENT_ASSERT(t.type == bdecode_token::string);

		return std::string(m_buffer + t.offset + t.start_offset(), size);
	}