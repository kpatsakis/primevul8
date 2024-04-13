innodb_conn_init(
/*=============*/
	innodb_engine_t*	engine,		/*!< in/out: InnoDB memcached
						engine */
	const void*		cookie,		/*!< in: This connection's
						cookie */
	int			conn_option,	/*!< in: whether it is
						for read or write operation*/
	ib_lck_mode_t		lock_mode,	/*!< in: Table lock mode */
	bool			has_lock,	/*!< in: Has engine mutex */
	meta_cfg_info_t*	new_meta_info)	/*!< in: meta info for
						table to open or NULL */
{
	innodb_conn_data_t*	conn_data;
	meta_cfg_info_t*	meta_info;
	meta_index_t*		meta_index;
	ib_err_t		err = DB_SUCCESS;
	ib_crsr_t		crsr;
	ib_crsr_t		read_crsr;
	ib_crsr_t		idx_crsr;
	bool			trx_updated = false;

	/* Get this connection's conn_data */
	conn_data = engine->server.cookie->get_engine_specific(cookie);

	assert(!conn_data || !conn_data->in_use);

	if (!conn_data) {
		LOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
		conn_data = engine->server.cookie->get_engine_specific(cookie);

		if (conn_data) {
                        UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
			goto have_conn;
		}

		if (UT_LIST_GET_LEN(engine->conn_data) > 2048) {
			/* Some of conn_data can be stale, recycle them */
			innodb_conn_clean(engine, false, true);
		}

		conn_data = malloc(sizeof(*conn_data));

		if (!conn_data) {
			UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
			return(NULL);
		}

		memset(conn_data, 0, sizeof(*conn_data));
		conn_data->result = malloc(sizeof(mci_item_t));
                if (!conn_data->result) {
			UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
			free(conn_data);
			conn_data = NULL;
			return(NULL);
		}
		conn_data->conn_meta = new_meta_info
					 ? new_meta_info
					 : engine->meta_info;
		conn_data->row_buf = malloc(1024);
                if (!conn_data->row_buf) {
			UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
			free(conn_data->result);
			free(conn_data);
			conn_data = NULL;
			return(NULL);
               }
		conn_data->row_buf_len = 1024;

		conn_data->cmd_buf = malloc(1024);
                if (!conn_data->cmd_buf) {
			UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
			free(conn_data->row_buf);
			free(conn_data->result);
			free(conn_data);
			conn_data = NULL;
			return(NULL);
               }
		conn_data->cmd_buf_len = 1024;

		conn_data->is_flushing = false;

		conn_data->conn_cookie = (void*) cookie;

		/* Add connection to the list after all memory allocations */
		UT_LIST_ADD_LAST(conn_list, engine->conn_data, conn_data);
		engine->server.cookie->store_engine_specific(
			cookie, conn_data);

		pthread_mutex_init(&conn_data->curr_conn_mutex, NULL);
		UNLOCK_CONN_IF_NOT_LOCKED(has_lock, engine);
	}
have_conn:
	meta_info = conn_data->conn_meta;
	meta_index = &meta_info->index_info;

	assert(engine->conn_data.count > 0);

	if (conn_option == CONN_MODE_NONE) {
		return(conn_data);
	}

	LOCK_CURRENT_CONN_IF_NOT_LOCKED(has_lock, conn_data);

	/* If flush is running, then wait for it complete. */
	if (conn_data->is_flushing) {
		/* Request flush_mutex for waiting for flush
		completed. */
		pthread_mutex_lock(&engine->flush_mutex);
		pthread_mutex_unlock(&engine->flush_mutex);
	}

       /* This special case added to facilitate unlocking
          of MDL lock during FLUSH TABLE WITH READ LOCK */
	if (engine &&
	    release_mdl_lock &&
	    (engine->enable_binlog || engine->enable_mdl)) {
		if ( DB_SUCCESS !=
			innodb_open_mysql_table(conn_data,
						conn_option,
						engine)){
			UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
				has_lock, conn_data);
			return NULL;
		}
	}

	conn_data->in_use = true;

	crsr = conn_data->crsr;
	read_crsr = conn_data->read_crsr;

	if (lock_mode == IB_LOCK_TABLE_X) {
		if(!conn_data->crsr_trx) {
			conn_data->crsr_trx = ib_cb_trx_begin(
				engine->trx_level, true, false);
		} else {
			/* Write cursor transaction exists.
			   Reuse this transaction.*/
			if (ib_cb_trx_read_only(conn_data->crsr_trx)) {
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
			}

			err = ib_cb_trx_start(conn_data->crsr_trx,
					      engine->trx_level,
					      true, false, NULL);
			assert(err == DB_SUCCESS);

		}

		err = innodb_api_begin(
			engine,
			meta_info->col_info[CONTAINER_DB].col_name,
			meta_info->col_info[CONTAINER_TABLE].col_name,
			conn_data, conn_data->crsr_trx,
			&conn_data->crsr, &conn_data->idx_crsr,
			lock_mode);

		if (err != DB_SUCCESS) {
			innodb_cb_cursor_close(
				conn_data->crsr);
			conn_data->crsr = NULL;
			innodb_cb_trx_commit(
				conn_data->crsr_trx);
			err = ib_cb_trx_release(conn_data->crsr_trx);
			assert(err == DB_SUCCESS);
			conn_data->crsr_trx = NULL;
			conn_data->in_use = false;
			UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
				has_lock, conn_data);
			return(NULL);
		}

		UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(has_lock, conn_data);
		return(conn_data);
	}

	/* Write operation */
	if (conn_option == CONN_MODE_WRITE) {
		if (!crsr) {
			if (!conn_data->crsr_trx) {
				conn_data->crsr_trx = ib_cb_trx_begin(
					engine->trx_level, true, false);
				trx_updated = true;
			} else {
				if (ib_cb_trx_read_only(conn_data->crsr_trx)) {
					innodb_cb_trx_commit(
						conn_data->crsr_trx);
				}

				ib_cb_trx_start(conn_data->crsr_trx,
						engine->trx_level,
						true, false, NULL);
			}

			err = innodb_api_begin(
				engine,
				meta_info->col_info[CONTAINER_DB].col_name,
				meta_info->col_info[CONTAINER_TABLE].col_name,
				conn_data, conn_data->crsr_trx,
				&conn_data->crsr, &conn_data->idx_crsr,
				lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->crsr);
				conn_data->crsr = NULL;
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->in_use = false;

				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);
				return(NULL);
			}

		} else if (!conn_data->crsr_trx) {

			/* There exists a cursor, just need update
			with a new transaction */
			conn_data->crsr_trx = ib_cb_trx_begin(
				engine->trx_level, true, false);

			innodb_cb_cursor_new_trx(crsr, conn_data->crsr_trx);
			trx_updated = true;

			err = innodb_cb_cursor_lock(engine, crsr, lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->crsr);
				conn_data->crsr = NULL;
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->in_use = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);
				return(NULL);
			}

			if (meta_index->srch_use_idx == META_USE_SECONDARY) {

				idx_crsr = conn_data->idx_crsr;
				innodb_cb_cursor_new_trx(
					idx_crsr, conn_data->crsr_trx);
				innodb_cb_cursor_lock(
					engine, idx_crsr, lock_mode);
			}
		} else {

			if (ib_cb_trx_read_only(conn_data->crsr_trx)) {
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
			}

			ib_cb_trx_start(conn_data->crsr_trx,
					engine->trx_level,
					true, false, NULL);
			ib_cb_cursor_stmt_begin(crsr);
			err = innodb_cb_cursor_lock(engine, crsr, lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->crsr);
				conn_data->crsr = NULL;
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->in_use = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);
				return(NULL);
			}
		}

		if (trx_updated) {
			if (conn_data->read_crsr) {
				innodb_cb_cursor_new_trx(
					conn_data->read_crsr,
					conn_data->crsr_trx);
			}

			if (conn_data->idx_read_crsr) {
				innodb_cb_cursor_new_trx(
					conn_data->idx_read_crsr,
					conn_data->crsr_trx);
			}
		}
	} else {
		assert(conn_option == CONN_MODE_READ);

		if (!read_crsr) {
			if (!conn_data->crsr_trx) {
				/* This is read operation, start a trx
				with "read_write" parameter set to false */
				conn_data->crsr_trx = ib_cb_trx_begin(
					engine->trx_level, false,
					engine->read_batch_size == 1);
				trx_updated = true;
			} else {
				ib_cb_trx_start(conn_data->crsr_trx,
						engine->trx_level,
						false,
						engine->read_batch_size == 1,
						NULL);
			}

			err = innodb_api_begin(
				engine,
				meta_info->col_info[CONTAINER_DB].col_name,
				meta_info->col_info[CONTAINER_TABLE].col_name,
				conn_data,
				conn_data->crsr_trx,
				&conn_data->read_crsr,
				&conn_data->idx_read_crsr,
				lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->read_crsr);
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->read_crsr = NULL;
				conn_data->in_use = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);

				return(NULL);
			}

		} else if (!conn_data->crsr_trx) {
			/* This is read operation, start a trx
			with "read_write" parameter set to false */
			conn_data->crsr_trx = ib_cb_trx_begin(
				engine->trx_level, false,
				engine->read_batch_size == 1);

			trx_updated = true;

			innodb_cb_cursor_new_trx(
				conn_data->read_crsr,
				conn_data->crsr_trx);

			if (conn_data->crsr) {
				innodb_cb_cursor_new_trx(
					conn_data->crsr,
					conn_data->crsr_trx);
			}

			err = innodb_cb_cursor_lock(
				engine, conn_data->read_crsr, lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->read_crsr);
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->read_crsr = NULL;
				conn_data->in_use = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);

				return(NULL);
                        }

			if (meta_index->srch_use_idx == META_USE_SECONDARY) {
				ib_crsr_t idx_crsr = conn_data->idx_read_crsr;

				innodb_cb_cursor_new_trx(
					idx_crsr, conn_data->crsr_trx);
				innodb_cb_cursor_lock(
					engine, idx_crsr, lock_mode);
			}
		} else {
			/* This is read operation, start a trx
			with "read_write" parameter set to false */
			ib_cb_trx_start(conn_data->crsr_trx,
					engine->trx_level,
					false,
					engine->read_batch_size == 1,
					NULL);

			ib_cb_cursor_stmt_begin(conn_data->read_crsr);

			err = innodb_cb_cursor_lock(
				engine, conn_data->read_crsr, lock_mode);

			if (err != DB_SUCCESS) {
				innodb_cb_cursor_close(
					conn_data->read_crsr);
				innodb_cb_trx_commit(
					conn_data->crsr_trx);
				err = ib_cb_trx_release(conn_data->crsr_trx);
				assert(err == DB_SUCCESS);
				conn_data->crsr_trx = NULL;
				conn_data->read_crsr = NULL;
				conn_data->in_use = false;
				UNLOCK_CURRENT_CONN_IF_NOT_LOCKED(
					has_lock, conn_data);

				return(NULL);
                        }

			if (meta_index->srch_use_idx == META_USE_SECONDARY) {
				ib_crsr_t idx_crsr = conn_data->idx_read_crsr;
				ib_cb_cursor_stmt_begin(idx_crsr);
				innodb_cb_cursor_lock(
					engine, idx_crsr, lock_mode);
			}
		}

		if (trx_updated) {
			if (conn_data->crsr) {
				innodb_cb_cursor_new_trx(
					conn_data->crsr,
					conn_data->crsr_trx);
			}

			if (conn_data->idx_crsr) {
				innodb_cb_cursor_new_trx(
					conn_data->idx_crsr,
					conn_data->crsr_trx);
			}
		}
	}

	UNLOCK_CURRENT_CONN_IF_NOT_LOCKED( has_lock, conn_data);

	return(conn_data);
}