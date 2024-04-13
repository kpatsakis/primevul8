parse_escape(uint8_t *ch_p, const char** str_p)
{
	uint16_t val;

	if ((*str_p)[0] && isdigit((unsigned char)(*str_p)[0])  &&
	    (*str_p)[1] && isdigit((unsigned char)(*str_p)[1])  &&
	    (*str_p)[2] && isdigit((unsigned char)(*str_p)[2]))  {

		val = (uint16_t)(((*str_p)[0] - '0') * 100 +
				 ((*str_p)[1] - '0') *  10 +
				 ((*str_p)[2] - '0'));

		if (val > 255) {
			goto error;
		}
		*ch_p = (uint8_t)val;
		*str_p += 3;
		return true;

	} else if ((*str_p)[0] && !isdigit((unsigned char)(*str_p)[0])) {

		*ch_p = (uint8_t)*(*str_p)++;
		return true;
	}
error:
	*str_p = NULL;
	return false; /* LDNS_STATUS_SYNTAX_BAD_ESCAPE */
}