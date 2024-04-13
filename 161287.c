static inline void alter_cred_subscribers(const struct cred *_cred, int n)
{
#ifdef CONFIG_DEBUG_CREDENTIALS
	struct cred *cred = (struct cred *) _cred;

	atomic_add(n, &cred->subscribers);
#endif
}