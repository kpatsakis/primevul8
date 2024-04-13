	void print_string(std::string& ret, char const* str, int len, bool single_line)
	{
		bool printable = true;
		for (int i = 0; i < len; ++i)
		{
			char c = str[i];
			if (c >= 32 && c < 127) continue;
			printable = false;
			break;
		}
		ret += "'";
		if (printable)
		{
			if (single_line && len > 30)
			{
				ret.append(str, 14);
				ret += "...";
				ret.append(str + len-14, 14);
			}
			else
				ret.append(str, len);
			ret += "'";
			return;
		}
		if (single_line && len > 20)
		{
			escape_string(ret, str, 9);
			ret += "...";
			escape_string(ret, str + len - 9, 9);
		}
		else
		{
			escape_string(ret, str, len);
		}
		ret += "'";
	}