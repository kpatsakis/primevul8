static inline int read_cred_subscribers(const struct cred *cred)
{
#ifdef CONFIG_DEBUG_CREDENTIALS
	return atomic_read(&cred->subscribers);
#else
	return 0;
#endif
}