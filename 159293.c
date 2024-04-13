static int spl_filesystem_is_invalid_or_dot(const char * d_name) /* {{{ */
{
	return d_name[0] == '\0' || spl_filesystem_is_dot(d_name);
}