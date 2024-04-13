innodb_conn_clean_data(
/*===================*/
	innodb_conn_data_t*	conn_data,
	bool			has_lock,
	bool			free_all)
{
	if (!conn_data) {
		return;
	}

	LOCK_CURRENT_CONN_IF_NOT_LOCKED(has_lock, conn_data);

	if (conn_data->idx_crsr) {
		innodb_cb_cursor_close(conn_data->idx_crsr);
		conn_data->idx_crsr = NULL;
	}

	if (conn_data->idx_read_crsr) {
		innodb_cb_cursor_close(conn_data->idx_read_crsr);
		conn_data->idx_read_crsr = NULL;
	}

	if (conn_data->crsr) {
		innodb_cb_cursor_close(conn_data->crsr);
		conn_data->crsr = NULL;
	}

	if (conn_data->read_crsr) {
		innodb_cb_cursor_close(conn_data->read_crsr);
		conn_data->read_crsr = NULL;
	}

	if (conn_data->crsr_trx) {
		ib_err_t        err;
		innodb_cb_trx_commit(conn_data->crsr_trx);
		err = ib_cb_trx_release(conn_data->crsr_trx);
		assert(err == DB_SUCCESS);
		conn_data->crsr_trx = NULL;
	}

	innodb_close_mysql_table(conn_data);

	if (conn_data->tpl) {
		ib_cb_tuple_delete(conn_data->tpl);
		conn_data->tpl = NULL;
	}

	if (conn_data->idx_tpl) {
		ib_cb_tuple_delete(conn_data->idx_tpl);
		conn_data->idx_tpl = NULL;
	}

	if (conn_data->read_tpl) {
		ib_cb_tuple_delete(conn_data->read_tpl);
		conn_data->read_tpl = NULL;
	}

	if (conn_data->sel_tpl) {
		ib_cb_tuple_delete(conn_data->sel_tpl);
		conn_data->sel_tpl = NULL;
	}

	UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(has_lock, conn_data);

	if (free_all) {
		if (conn_data->result) {
			free(conn_data->result);
			conn_data->result = NULL;
		}

		if (conn_data->row_buf) {
			free(conn_data->row_buf);
			conn_data->row_buf = NULL;
			conn_data->row_buf_len = 0;
		}

		if (conn_data->cmd_buf) {
			free(conn_data->cmd_buf);
			conn_data->cmd_buf = NULL;
			conn_data->cmd_buf_len = 0;
		}

		if (conn_data->mul_col_buf) {
			free(conn_data->mul_col_buf);
			conn_data->mul_col_buf = NULL;
			conn_data->mul_col_buf_len = 0;
		}

		pthread_mutex_destroy(&conn_data->curr_conn_mutex);
		free(conn_data);
	}
}