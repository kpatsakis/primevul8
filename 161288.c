static int construct_alloc_key(struct key_type *type,
			       const char *description,
			       struct key *dest_keyring,
			       unsigned long flags,
			       struct key_user *user,
			       struct key **_key)
{
	const struct cred *cred = current_cred();
	unsigned long prealloc;
	struct key *key;
	key_ref_t key_ref;
	int ret;

	kenter("%s,%s,,,", type->name, description);

	*_key = NULL;
	mutex_lock(&user->cons_lock);

	key = key_alloc(type, description, cred->fsuid, cred->fsgid, cred,
			KEY_POS_ALL, flags);
	if (IS_ERR(key))
		goto alloc_failed;

	set_bit(KEY_FLAG_USER_CONSTRUCT, &key->flags);

	if (dest_keyring) {
		ret = __key_link_begin(dest_keyring, type, description,
				       &prealloc);
		if (ret < 0)
			goto link_prealloc_failed;
	}

	/* attach the key to the destination keyring under lock, but we do need
	 * to do another check just in case someone beat us to it whilst we
	 * waited for locks */
	mutex_lock(&key_construction_mutex);

	key_ref = search_process_keyrings(type, description, type->match, cred);
	if (!IS_ERR(key_ref))
		goto key_already_present;

	if (dest_keyring)
		__key_link(dest_keyring, key, &prealloc);

	mutex_unlock(&key_construction_mutex);
	if (dest_keyring)
		__key_link_end(dest_keyring, type, prealloc);
	mutex_unlock(&user->cons_lock);
	*_key = key;
	kleave(" = 0 [%d]", key_serial(key));
	return 0;

	/* the key is now present - we tell the caller that we found it by
	 * returning -EINPROGRESS  */
key_already_present:
	key_put(key);
	mutex_unlock(&key_construction_mutex);
	key = key_ref_to_ptr(key_ref);
	if (dest_keyring) {
		ret = __key_link_check_live_key(dest_keyring, key);
		if (ret == 0)
			__key_link(dest_keyring, key, &prealloc);
		__key_link_end(dest_keyring, type, prealloc);
		if (ret < 0)
			goto link_check_failed;
	}
	mutex_unlock(&user->cons_lock);
	*_key = key;
	kleave(" = -EINPROGRESS [%d]", key_serial(key));
	return -EINPROGRESS;

link_check_failed:
	mutex_unlock(&user->cons_lock);
	key_put(key);
	kleave(" = %d [linkcheck]", ret);
	return ret;

link_prealloc_failed:
	mutex_unlock(&user->cons_lock);
	kleave(" = %d [prelink]", ret);
	return ret;

alloc_failed:
	mutex_unlock(&user->cons_lock);
	kleave(" = %ld", PTR_ERR(key));
	return PTR_ERR(key);
}