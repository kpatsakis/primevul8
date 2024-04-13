innodb_open_mysql_table(
/*====================*/
	innodb_conn_data_t*     conn_data,	/*!< in/out:Connection cursor data */
	int                     conn_option,	/*!< in: Read or write operation */
	innodb_engine_t*        engine)		/*!< in: InnoDB memcached engine */
{
	meta_cfg_info_t*        meta_info;
	meta_info = conn_data->conn_meta;
	conn_data->is_waiting_for_mdl = true;

	/* Close the table before opening it again */
	innodb_close_mysql_table(conn_data);

	if (conn_option == CONN_MODE_READ) {
               conn_data->is_waiting_for_mdl = false;
               return (DB_SUCCESS);
	}

	if (!conn_data->thd) {
		conn_data->thd = handler_create_thd(engine->enable_binlog);
		if (!conn_data->thd) {
			return(DB_ERROR);
		}
	}

	if (!conn_data->mysql_tbl) {
		conn_data->mysql_tbl =
			handler_open_table(
				conn_data->thd,
				meta_info->col_info[CONTAINER_DB].col_name,
				meta_info->col_info[CONTAINER_TABLE].col_name,
				HDL_WRITE);
	}
	conn_data->is_waiting_for_mdl = false;

	if(!conn_data->mysql_tbl) {
		return(DB_LOCK_WAIT);
	}

	return (DB_SUCCESS);
}