static struct key *construct_key_and_link(struct key_type *type,
					  const char *description,
					  const char *callout_info,
					  size_t callout_len,
					  void *aux,
					  struct key *dest_keyring,
					  unsigned long flags)
{
	struct key_user *user;
	struct key *key;
	int ret;

	kenter("");

	user = key_user_lookup(current_fsuid(), current_user_ns());
	if (!user)
		return ERR_PTR(-ENOMEM);

	construct_get_dest_keyring(&dest_keyring);

	ret = construct_alloc_key(type, description, dest_keyring, flags, user,
				  &key);
	key_user_put(user);

	if (ret == 0) {
		ret = construct_key(key, callout_info, callout_len, aux,
				    dest_keyring);
		if (ret < 0) {
			kdebug("cons failed");
			goto construction_failed;
		}
	} else if (ret == -EINPROGRESS) {
		ret = 0;
	} else {
		goto couldnt_alloc_key;
	}

	key_put(dest_keyring);
	kleave(" = key %d", key_serial(key));
	return key;

construction_failed:
	key_negate_and_link(key, key_negative_timeout, NULL, NULL);
	key_put(key);
couldnt_alloc_key:
	key_put(dest_keyring);
	kleave(" = %d", ret);
	return ERR_PTR(ret);
}