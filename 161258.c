static inline void set_cred_subscribers(struct cred *cred, int n)
{
#ifdef CONFIG_DEBUG_CREDENTIALS
	atomic_set(&cred->subscribers, n);
#endif
}