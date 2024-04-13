	bdecode_node::type_t bdecode_node::type() const
	{
		if (m_token_idx == -1) return none_t;
		return static_cast<bdecode_node::type_t>(m_root_tokens[m_token_idx].type);
	}