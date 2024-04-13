bool __jbd2_journal_refile_buffer(struct journal_head *jh)
{
	int was_dirty, jlist;
	struct buffer_head *bh = jh2bh(jh);

	lockdep_assert_held(&jh->b_state_lock);
	if (jh->b_transaction)
		assert_spin_locked(&jh->b_transaction->t_journal->j_list_lock);

	/* If the buffer is now unused, just drop it. */
	if (jh->b_next_transaction == NULL) {
		__jbd2_journal_unfile_buffer(jh);
		return true;
	}

	/*
	 * It has been modified by a later transaction: add it to the new
	 * transaction's metadata list.
	 */

	was_dirty = test_clear_buffer_jbddirty(bh);
	__jbd2_journal_temp_unlink_buffer(jh);

	/*
	 * b_transaction must be set, otherwise the new b_transaction won't
	 * be holding jh reference
	 */
	J_ASSERT_JH(jh, jh->b_transaction != NULL);

	/*
	 * We set b_transaction here because b_next_transaction will inherit
	 * our jh reference and thus __jbd2_journal_file_buffer() must not
	 * take a new one.
	 */
	WRITE_ONCE(jh->b_transaction, jh->b_next_transaction);
	WRITE_ONCE(jh->b_next_transaction, NULL);
	if (buffer_freed(bh))
		jlist = BJ_Forget;
	else if (jh->b_modified)
		jlist = BJ_Metadata;
	else
		jlist = BJ_Reserved;
	__jbd2_journal_file_buffer(jh, jh->b_transaction, jlist);
	J_ASSERT_JH(jh, jh->b_transaction->t_state == T_RUNNING);

	if (was_dirty)
		set_buffer_jbddirty(bh);
	return false;
}