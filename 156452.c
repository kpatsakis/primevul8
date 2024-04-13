	boost::int64_t bdecode_node::int_value() const
	{
		TORRENT_ASSERT(type() == int_t);
		bdecode_token const& t = m_root_tokens[m_token_idx];
		int size = m_root_tokens[m_token_idx + 1].offset - t.offset;
		TORRENT_ASSERT(t.type == bdecode_token::integer);
	
		// +1 is to skip the 'i'
		char const* ptr = m_buffer + t.offset + 1;
		boost::int64_t val = 0;
		bool negative = false;
		if (*ptr == '-') negative = true;
		bdecode_errors::error_code_enum ec = bdecode_errors::no_error;
		parse_int(ptr + negative
			, ptr + size, 'e', val, ec);
		if (ec) return 0;
		if (negative) val = -val;
		return val;
	}