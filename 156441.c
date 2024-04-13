	void bdecode_node::clear()
	{
		m_tokens.clear();
		m_root_tokens = NULL;
		m_token_idx = -1;
		m_size = -1;
		m_last_index = -1;
		m_last_token = -1;
	}