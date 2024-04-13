	bdecode_node& bdecode_node::operator=(bdecode_node const& n)
	{
		m_tokens = n.m_tokens;
		m_root_tokens = n.m_root_tokens;
		m_buffer = n.m_buffer;
		m_buffer_size = n.m_buffer_size;
		m_token_idx = n.m_token_idx;
		m_last_index = n.m_last_index;
		m_last_token = n.m_last_token;
		m_size = n.m_size;
		if (!m_tokens.empty())
		{
			// if this is a root, make the token pointer
			// point to our storage
			m_root_tokens = &m_tokens[0];
		}
		return *this;
	}