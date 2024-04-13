mm_auth_rhosts_rsa_key_allowed(struct passwd *pw, char *user,
    char *host, Key *key)
{
	int ret;

	key->type = KEY_RSA; /* XXX hack for key_to_blob */
	ret = mm_key_allowed(MM_RSAHOSTKEY, user, host, key);
	key->type = KEY_RSA1;
	return (ret);
}