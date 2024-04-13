 * but only some parts of it are used. */
static void set_packet_error(
		MYSQLND_ERROR_INFO *info, unsigned err_no, const char *sqlstate, const char *error)
{
	info->error_no = err_no;
	strlcpy(info->sqlstate, sqlstate, sizeof(info->sqlstate));
	strlcpy(info->error, error, sizeof(info->error));