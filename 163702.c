find_url_param_value(char* path, size_t path_l,
                     char* url_param_name, size_t url_param_name_l,
                     char** value, int* value_l, char delim)
{
	char *query_string, *qs_end;
	char *arg_start;
	char *value_start, *value_end;

	query_string = find_param_list(path, path_l, delim);
	if (!query_string)
		return 0;

	qs_end = path + path_l;
	arg_start = find_url_param_pos(query_string, qs_end - query_string,
                                      url_param_name, url_param_name_l,
                                      delim);
	if (!arg_start)
		return 0;

	value_start = arg_start + url_param_name_l + 1;
	value_end = value_start;

	while ((value_end < qs_end) && !is_param_delimiter(*value_end, delim))
		value_end++;

	*value = value_start;
	*value_l = value_end - value_start;
	return value_end != value_start;
}