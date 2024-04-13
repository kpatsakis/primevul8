		boost::system::error_code make_error_code(error_code_enum e)
		{
			return boost::system::error_code(e, bdecode_category());
		}