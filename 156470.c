	char const* check_integer(char const* start, char const* end
		, bdecode_errors::error_code_enum& e)
	{
		if (start == end)
		{
			e = bdecode_errors::unexpected_eof;
			return start;
		}

		if (*start == '-')
		{
			++start;
			if (start == end)
			{
				e = bdecode_errors::unexpected_eof;
				return start;
			}
		}

		int digits = 0;
		do
		{
			if (!numeric(*start))
			{
				e = bdecode_errors::expected_digit;
				break;
			}
			++start;
			++digits;

			if (start == end)
			{
				e = bdecode_errors::unexpected_eof;
				break;
			}
		}
		while (*start != 'e');

		if (digits > 20)
		{
			e = bdecode_errors::overflow;
		}

		return start;
	}