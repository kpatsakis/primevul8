innodb_get_info(
/*============*/
	ENGINE_HANDLE*	handle)		/*!< in: Engine handle */
{
	return(&innodb_handle(handle)->info.info);
}