innodb_close_mysql_table(
/*=====================*/
	innodb_conn_data_t*     conn_data)	/*!< in: connection
							 cursor*/
{
	if (conn_data->mysql_tbl) {
		assert(conn_data->thd);
		handler_unlock_table(conn_data->thd,
				     conn_data->mysql_tbl,
				     HDL_READ);
                conn_data->mysql_tbl = NULL;
	}

	if (conn_data->thd) {
		handler_close_thd(conn_data->thd);
		conn_data->thd = NULL;
	}
}