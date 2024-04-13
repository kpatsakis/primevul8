convert_to_char(
/*============*/
	char*		buf,		/*!< out: converted integer value */
	int		buf_len,	/*!< in: buffer len */
	void*		value,		/*!< in: int value */
	int		value_len,	/*!< in: int len */
	bool		is_unsigned)	/*!< in: whether it is unsigned */
{
	assert(buf && buf_len);

	if (value_len == 8) {
		if (is_unsigned) {
			uint64_t	int_val = *(uint64_t*)value;
			snprintf(buf, buf_len, "%" PRIu64, int_val);
		} else {
			int64_t		int_val = *(int64_t*)value;
			snprintf(buf, buf_len, "%" PRIi64, int_val);
		}
	} else if (value_len == 4) {
		if (is_unsigned) {
			uint32_t	int_val = *(uint32_t*)value;
			snprintf(buf, buf_len, "%" PRIu32, int_val);
		} else {
			int32_t		int_val = *(int32_t*)value;
			snprintf(buf, buf_len, "%" PRIi32, int_val);
		}
	} else if (value_len == 2) {
		if (is_unsigned) {
			uint16_t	int_val = *(uint16_t*)value;
			snprintf(buf, buf_len, "%" PRIu16, int_val);
		} else {
			int16_t		int_val = *(int16_t*)value;
			snprintf(buf, buf_len, "%" PRIi16, int_val);
		}
	} else if (value_len == 1) {
		if (is_unsigned) {
			uint8_t		int_val = *(uint8_t*)value;
			snprintf(buf, buf_len, "%" PRIu8, int_val);
		} else {
			int8_t		int_val = *(int8_t*)value;
			snprintf(buf, buf_len, "%" PRIi8, int_val);
		}
	}

	return(strlen(buf));
}