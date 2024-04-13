	bool string_equal(char const* str1, char const* str2, int len2)
	{
		while (len2 > 0)
		{
			if (*str1 != *str2) return false;
			if (*str1 == 0) return false;
			++str1;
			++str2;
			--len2;
		}
		return *str1 == 0;
	}