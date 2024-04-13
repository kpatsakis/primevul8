__journal_try_to_free_buffer(journal_t *journal, struct buffer_head *bh)
{
	struct journal_head *jh;

	jh = bh2jh(bh);

	if (buffer_locked(bh) || buffer_dirty(bh))
		goto out;

	if (jh->b_next_transaction != NULL || jh->b_transaction != NULL)
		goto out;

	spin_lock(&journal->j_list_lock);
	if (jh->b_cp_transaction != NULL) {
		/* written-back checkpointed metadata buffer */
		JBUFFER_TRACE(jh, "remove from checkpoint list");
		__jbd2_journal_remove_checkpoint(jh);
	}
	spin_unlock(&journal->j_list_lock);
out:
	return;
}