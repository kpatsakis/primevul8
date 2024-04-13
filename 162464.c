innodb_bind(
/*========*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine handle */
	const void*		cookie,		/*!< in: connection cookie */
	const void*		name,		/*!< in: table ID name */
	size_t			name_len)	/*!< in: name length */
{
	ENGINE_ERROR_CODE	err_ret = ENGINE_SUCCESS;

	err_ret = innodb_switch_mapping(handle, cookie, name, &name_len, false);

	return(err_ret);
}