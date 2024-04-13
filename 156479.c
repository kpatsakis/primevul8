	std::pair<char const*, int> bdecode_node::data_section() const
	{
		if (m_token_idx == -1) return std::make_pair(m_buffer, 0);

		TORRENT_ASSERT(m_token_idx != -1);
		bdecode_token const& t = m_root_tokens[m_token_idx];
		bdecode_token const& next = m_root_tokens[m_token_idx + t.next_item];
		return std::make_pair(m_buffer + t.offset, next.offset - t.offset);
	}