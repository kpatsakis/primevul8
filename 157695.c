void jbd2_journal_unfile_buffer(journal_t *journal, struct journal_head *jh)
{
	struct buffer_head *bh = jh2bh(jh);

	/* Get reference so that buffer cannot be freed before we unlock it */
	get_bh(bh);
	spin_lock(&jh->b_state_lock);
	spin_lock(&journal->j_list_lock);
	__jbd2_journal_unfile_buffer(jh);
	spin_unlock(&journal->j_list_lock);
	spin_unlock(&jh->b_state_lock);
	jbd2_journal_put_journal_head(jh);
	__brelse(bh);
}