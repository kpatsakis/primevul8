struct key *request_key_and_link(struct key_type *type,
				 const char *description,
				 const void *callout_info,
				 size_t callout_len,
				 void *aux,
				 struct key *dest_keyring,
				 unsigned long flags)
{
	const struct cred *cred = current_cred();
	struct key *key;
	key_ref_t key_ref;
	int ret;

	kenter("%s,%s,%p,%zu,%p,%p,%lx",
	       type->name, description, callout_info, callout_len, aux,
	       dest_keyring, flags);

	/* search all the process keyrings for a key */
	key_ref = search_process_keyrings(type, description, type->match, cred);

	if (!IS_ERR(key_ref)) {
		key = key_ref_to_ptr(key_ref);
		if (dest_keyring) {
			construct_get_dest_keyring(&dest_keyring);
			ret = key_link(dest_keyring, key);
			key_put(dest_keyring);
			if (ret < 0) {
				key_put(key);
				key = ERR_PTR(ret);
				goto error;
			}
		}
	} else if (PTR_ERR(key_ref) != -EAGAIN) {
		key = ERR_CAST(key_ref);
	} else  {
		/* the search failed, but the keyrings were searchable, so we
		 * should consult userspace if we can */
		key = ERR_PTR(-ENOKEY);
		if (!callout_info)
			goto error;

		key = construct_key_and_link(type, description, callout_info,
					     callout_len, aux, dest_keyring,
					     flags);
	}

error:
	kleave(" = %p", key);
	return key;
}