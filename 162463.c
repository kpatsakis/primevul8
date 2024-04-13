innodb_get_item_info(
/*=================*/
	ENGINE_HANDLE*		handle __attribute__((unused)),
						/*!< in: Engine Handle */
	const void*		cookie __attribute__((unused)),
						/*!< in: connection cookie */
	const item*		item,		/*!< in: item in question */
	item_info*		item_info)	/*!< out: item info got */
{
	struct innodb_engine* innodb_eng = innodb_handle(handle);
	innodb_conn_data_t*     conn_data;

	conn_data = innodb_eng->server.cookie->get_engine_specific(cookie);

	if (!conn_data || !conn_data->result_in_use) {
		hash_item*      it;

		if (item_info->nvalue < 1) {
			return(false);
		}

		/* Use a hash item */
		it = (hash_item*) item;
		item_info->cas = hash_item_get_cas(it);
		item_info->exptime = it->exptime;
		item_info->nbytes = it->nbytes;
		item_info->flags = it->flags;
		item_info->clsid = it->slabs_clsid;
		item_info->nkey = it->nkey;
		item_info->nvalue = 1;
		item_info->key = hash_item_get_key(it);
		item_info->value[0].iov_base = hash_item_get_data(it);
		item_info->value[0].iov_len = it->nbytes;
	} else {
		mci_item_t*	it;

		if (item_info->nvalue < 1) {
			return(false);
		}

		/* Use a hash item */
		it = (mci_item_t*) item;
		if (it->col_value[MCI_COL_CAS].is_valid) {
			item_info->cas = it->col_value[MCI_COL_CAS].value_int;
		} else {
			item_info->cas = 0;
		}

		if (it->col_value[MCI_COL_EXP].is_valid) {
			item_info->exptime = it->col_value[MCI_COL_EXP].value_int;
		} else {
			item_info->exptime = 0;
		}

		item_info->nbytes = it->col_value[MCI_COL_VALUE].value_len;

		if (it->col_value[MCI_COL_FLAG].is_valid) {
			item_info->flags = ntohl(
				it->col_value[MCI_COL_FLAG].value_int);
		} else {
			item_info->flags = 0;
		}

		item_info->clsid = 1;

		item_info->nkey = it->col_value[MCI_COL_KEY].value_len;

		item_info->nvalue = 1;

		item_info->key = it->col_value[MCI_COL_KEY].value_str;

		item_info->value[0].iov_base = it->col_value[
					       MCI_COL_VALUE].value_str;;

		item_info->value[0].iov_len = it->col_value[
						MCI_COL_VALUE].value_len;

	}

	return(true);
}