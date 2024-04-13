innodb_free_item(
/*=====================*/
	void* item)	/*!< in: Item to be freed */
{

	mci_item_t*	result = (mci_item_t*) item;
	if (result->extra_col_value) {
		for (int i = 0; i < result->n_extra_col; i++) {
			if(result->extra_col_value[i].allocated)
				free(result->extra_col_value[i].value_str);
			}
			free(result->extra_col_value);
			result->extra_col_value=NULL;
		}
	if (result->col_value[MCI_COL_VALUE].allocated) {
		free(result->col_value[MCI_COL_VALUE].value_str);
		result->col_value[MCI_COL_VALUE].allocated =
			false;
	}
}