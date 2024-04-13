innodb_clean_engine(
/*================*/
	ENGINE_HANDLE*		handle,		/*!< in: Engine handle */
	const void*		cookie __attribute__((unused)),
						/*!< in: connection cookie */
	void*			conn)		/*!< in: item to free */
{
	innodb_conn_data_t*	conn_data = (innodb_conn_data_t*)conn;
	struct innodb_engine*	engine = innodb_handle(handle);
	void*			orignal_thd;

	LOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
	if (conn_data->thd) {
		handler_thd_attach(conn_data->thd, &orignal_thd);
	}
	innodb_reset_conn(conn_data, true, true, engine->enable_binlog);
	innodb_conn_clean_data(conn_data, true, false);
	conn_data->is_stale = true;
	UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(false, conn_data);
}