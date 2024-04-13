find_url_param_pos(char* query_string, size_t query_string_l,
                   char* url_param_name, size_t url_param_name_l,
                   char delim)
{
	char *pos, *last;

	pos  = query_string;
	last = query_string + query_string_l - url_param_name_l - 1;

	while (pos <= last) {
		if (pos[url_param_name_l] == '=') {
			if (memcmp(pos, url_param_name, url_param_name_l) == 0)
				return pos;
			pos += url_param_name_l + 1;
		}
		while (pos <= last && !is_param_delimiter(*pos, delim))
			pos++;
		pos++;
	}
	return NULL;
}