static void __jbd2_journal_unreserve_handle(handle_t *handle, transaction_t *t)
{
	journal_t *journal = handle->h_journal;

	WARN_ON(!handle->h_reserved);
	sub_reserved_credits(journal, handle->h_total_credits);
	if (t)
		atomic_sub(handle->h_total_credits, &t->t_outstanding_credits);
}