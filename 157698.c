void jbd2_journal_refile_buffer(journal_t *journal, struct journal_head *jh)
{
	bool drop;

	spin_lock(&jh->b_state_lock);
	spin_lock(&journal->j_list_lock);
	drop = __jbd2_journal_refile_buffer(jh);
	spin_unlock(&jh->b_state_lock);
	spin_unlock(&journal->j_list_lock);
	if (drop)
		jbd2_journal_put_journal_head(jh);
}