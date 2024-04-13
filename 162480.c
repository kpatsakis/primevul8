innodb_handle(
/*==========*/
	ENGINE_HANDLE*	handle)		/*!< in: Generic engine handle */
{
	return((struct innodb_engine*) handle);
}