void jbd2_journal_free_reserved(handle_t *handle)
{
	journal_t *journal = handle->h_journal;

	/* Get j_state_lock to pin running transaction if it exists */
	read_lock(&journal->j_state_lock);
	__jbd2_journal_unreserve_handle(handle, journal->j_running_transaction);
	read_unlock(&journal->j_state_lock);
	jbd2_free_handle(handle);
}