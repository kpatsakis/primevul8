	bdecode_node::bdecode_node(bdecode_token const* tokens, char const* buf
		, int len, int idx)
		: m_root_tokens(tokens)
		, m_buffer(buf)
		, m_buffer_size(len)
		, m_token_idx(idx)
		, m_last_index(-1)
		, m_last_token(-1)
		, m_size(-1)
	{
		TORRENT_ASSERT(tokens != NULL);
		TORRENT_ASSERT(idx >= 0);
	}