void jbd2_journal_file_buffer(struct journal_head *jh,
				transaction_t *transaction, int jlist)
{
	spin_lock(&jh->b_state_lock);
	spin_lock(&transaction->t_journal->j_list_lock);
	__jbd2_journal_file_buffer(jh, transaction, jlist);
	spin_unlock(&transaction->t_journal->j_list_lock);
	spin_unlock(&jh->b_state_lock);
}