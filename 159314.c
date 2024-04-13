static inline int spl_filesystem_is_dot(const char * d_name) /* {{{ */
{
	return !strcmp(d_name, ".") || !strcmp(d_name, "..");
}