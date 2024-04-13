	std::string print_entry(bdecode_node const& e
		, bool single_line, int indent)
	{
		char indent_str[200];
		using std::memset;
		memset(indent_str, ' ', 200);
		indent_str[0] = ',';
		indent_str[1] = '\n';
		indent_str[199] = 0;
		if (indent < 197 && indent >= 0) indent_str[indent+2] = 0;
		std::string ret;
		switch (e.type())
		{
			case bdecode_node::none_t: return "none";
			case bdecode_node::int_t:
			{
				char str[100];
				snprintf(str, sizeof(str), "%" PRId64, e.int_value());
				return str;
			}
			case bdecode_node::string_t:
			{
				print_string(ret, e.string_ptr(), e.string_length(), single_line);
				return ret;
			}
			case bdecode_node::list_t:
			{
				ret += '[';
				bool one_liner = line_longer_than(e, 200) != -1 || single_line;

				if (!one_liner) ret += indent_str + 1;
				for (int i = 0; i < e.list_size(); ++i)
				{
					if (i == 0 && one_liner) ret += " ";
					ret += print_entry(e.list_at(i), single_line, indent + 2);
					if (i < e.list_size() - 1) ret += (one_liner?", ":indent_str);
					else ret += (one_liner?" ":indent_str+1);
				}
				ret += "]";
				return ret;
			}
			case bdecode_node::dict_t:
			{
				ret += "{";
				bool one_liner = line_longer_than(e, 200) != -1 || single_line;

				if (!one_liner) ret += indent_str+1;
				for (int i = 0; i < e.dict_size(); ++i)
				{
					if (i == 0 && one_liner) ret += " ";
					std::pair<std::string, bdecode_node> ent = e.dict_at(i);
					print_string(ret, ent.first.c_str(), ent.first.size(), true);
					ret += ": ";
					ret += print_entry(ent.second, single_line, indent + 2);
					if (i < e.dict_size() - 1) ret += (one_liner?", ":indent_str);
					else ret += (one_liner?" ":indent_str+1);
				}
				ret += "}";
				return ret;
			}
		}
		return ret;
	}