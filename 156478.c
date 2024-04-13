	bdecode_node bdecode_node::non_owning() const
	{
		// if we're not a root, just return a copy of ourself
		if (m_tokens.empty()) return *this;

		// otherwise, return a reference to this node, but without
		// being an owning root node
		return bdecode_node(&m_tokens[0], m_buffer, m_buffer_size, m_token_idx);
	}