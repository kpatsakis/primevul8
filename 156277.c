mm_user_key_allowed(struct passwd *pw, Key *key)
{
	return (mm_key_allowed(MM_USERKEY, NULL, NULL, key));
}