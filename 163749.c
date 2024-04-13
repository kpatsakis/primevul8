static inline int is_param_delimiter(char c, char delim)
{
	return c == '&' || c == ';' || c == delim;
}