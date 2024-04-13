	void escape_string(std::string& ret, char const* str, int len)
	{
		for (int i = 0; i < len; ++i)
		{
			if (str[i] >= 32 && str[i] < 127)
			{
				ret += str[i];
			}
			else
			{
				char tmp[5];
				snprintf(tmp, sizeof(tmp), "\\x%02x", boost::uint8_t(str[i]));
				ret += tmp;
			}
		}
	}