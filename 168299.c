PHPAPI void var_push_dtor(php_unserialize_data_t *var_hashx, zval **rval)
{
	var_entries *var_hash = var_hashx->first_dtor, *prev = NULL;

	while (var_hash && var_hash->used_slots == VAR_ENTRIES_MAX) {
		prev = var_hash;
		var_hash = var_hash->next;
	}

	if (!var_hash) {
		var_hash = emalloc(sizeof(var_entries));
		var_hash->used_slots = 0;
		var_hash->next = 0;

		if (!var_hashx->first_dtor)
			var_hashx->first_dtor = var_hash;
		else
			prev->next = var_hash;
	}

	Z_ADDREF_PP(rval);
	var_hash->data[var_hash->used_slots++] = *rval;
}