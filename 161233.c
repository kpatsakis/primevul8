int install_thread_keyring_to_cred(struct cred *new)
{
	struct key *keyring;

	keyring = keyring_alloc("_tid", new->uid, new->gid, new,
				KEY_ALLOC_QUOTA_OVERRUN, NULL);
	if (IS_ERR(keyring))
		return PTR_ERR(keyring);

	new->thread_keyring = keyring;
	return 0;
}