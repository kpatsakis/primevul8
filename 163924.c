parse_char(uint8_t *ch_p, const char** str_p)
{
	switch (**str_p) {

	case '\0':	return false;

	case '\\':	*str_p += 1;
			return parse_escape(ch_p, str_p);

	default:	*ch_p = (uint8_t)*(*str_p)++;
			return true;
	}
}