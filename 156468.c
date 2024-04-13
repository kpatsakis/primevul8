	void bdecode_node::swap(bdecode_node& n)
	{
/*
		bool lhs_is_root = (m_root_tokens == &m_tokens);
		bool rhs_is_root = (n.m_root_tokens == &n.m_tokens);

		// swap is only defined between non-root nodes
		// and between root-nodes. They may not be mixed!
		// note that when swapping root nodes, all bdecode_node
		// entries that exist in those subtrees are invalidated!
		TORRENT_ASSERT(lhs_is_root == rhs_is_root);

		// if both are roots, m_root_tokens always point to
		// its own vector, and should not get swapped (the
		// underlying vectors are swapped already)
		if (!lhs_is_root && !rhs_is_root)
		{
			// if neither is a root, we just swap the pointers
			// to the token vectors, switching their roots
			std::swap(m_root_tokens, n.m_root_tokens);
		}
*/
		m_tokens.swap(n.m_tokens);
		std::swap(m_root_tokens, n.m_root_tokens);
		std::swap(m_buffer, n.m_buffer);
		std::swap(m_buffer_size, n.m_buffer_size);
		std::swap(m_token_idx, n.m_token_idx);
		std::swap(m_last_index, n.m_last_index);
		std::swap(m_last_token, n.m_last_token);
		std::swap(m_size, n.m_size);
	}