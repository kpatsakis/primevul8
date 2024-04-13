int __init jbd2_journal_init_transaction_cache(void)
{
	J_ASSERT(!transaction_cache);
	transaction_cache = kmem_cache_create("jbd2_transaction_s",
					sizeof(transaction_t),
					0,
					SLAB_HWCACHE_ALIGN|SLAB_TEMPORARY,
					NULL);
	if (!transaction_cache) {
		pr_emerg("JBD2: failed to create transaction cache\n");
		return -ENOMEM;
	}
	return 0;
}