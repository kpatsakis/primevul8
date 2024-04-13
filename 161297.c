long keyctl_invalidate_key(key_serial_t id)
{
	key_ref_t key_ref;
	long ret;

	kenter("%d", id);

	key_ref = lookup_user_key(id, 0, KEY_SEARCH);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		goto error;
	}

	key_invalidate(key_ref_to_ptr(key_ref));
	ret = 0;

	key_ref_put(key_ref);
error:
	kleave(" = %ld", ret);
	return ret;
}