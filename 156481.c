	void bdecode_node::switch_underlying_buffer(char const* buf)
	{
		TORRENT_ASSERT(!m_tokens.empty());
		if (m_tokens.empty()) return;

		m_buffer = buf;
	}