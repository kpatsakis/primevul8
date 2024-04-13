	int line_longer_than(bdecode_node const& e, int limit)
	{
		int line_len = 0;
		switch (e.type())
		{
		case bdecode_node::list_t:
			line_len += 4;
			if (line_len > limit) return -1;
			for (int i = 0; i < e.list_size(); ++i)
			{
				int ret = line_longer_than(e.list_at(i), limit - line_len);
				if (ret == -1) return -1;
				line_len += ret + 2;
			}
			break;
		case bdecode_node::dict_t:
			line_len += 4;
			if (line_len > limit) return -1;
			for (int i = 0; i < e.dict_size(); ++i)
			{
				line_len += 4 + e.dict_at(i).first.size();
				if (line_len > limit) return -1;
				int ret = line_longer_than(e.dict_at(i).second, limit - line_len);
				if (ret == -1) return -1;
				line_len += ret + 1;
			}
			break;
		case bdecode_node::string_t:
			line_len += 3 + e.string_length();
			break;
		case bdecode_node::int_t:
		{
			boost::int64_t val = e.int_value();
			while (val > 0)
			{
				++line_len;
				val /= 10;
			}
			line_len += 2;
		}
		break;
		case bdecode_node::none_t:
			line_len += 4;
			break;
		}

		if (line_len > limit) return -1;
		return line_len;
	}