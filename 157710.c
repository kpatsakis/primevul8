static void stop_this_handle(handle_t *handle)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal = transaction->t_journal;
	int revokes;

	J_ASSERT(journal_current_handle() == handle);
	J_ASSERT(atomic_read(&transaction->t_updates) > 0);
	current->journal_info = NULL;
	/*
	 * Subtract necessary revoke descriptor blocks from handle credits. We
	 * take care to account only for revoke descriptor blocks the
	 * transaction will really need as large sequences of transactions with
	 * small numbers of revokes are relatively common.
	 */
	revokes = handle->h_revoke_credits_requested - handle->h_revoke_credits;
	if (revokes) {
		int t_revokes, revoke_descriptors;
		int rr_per_blk = journal->j_revoke_records_per_block;

		WARN_ON_ONCE(DIV_ROUND_UP(revokes, rr_per_blk)
				> handle->h_total_credits);
		t_revokes = atomic_add_return(revokes,
				&transaction->t_outstanding_revokes);
		revoke_descriptors =
			DIV_ROUND_UP(t_revokes, rr_per_blk) -
			DIV_ROUND_UP(t_revokes - revokes, rr_per_blk);
		handle->h_total_credits -= revoke_descriptors;
	}
	atomic_sub(handle->h_total_credits,
		   &transaction->t_outstanding_credits);
	if (handle->h_rsv_handle)
		__jbd2_journal_unreserve_handle(handle->h_rsv_handle,
						transaction);
	if (atomic_dec_and_test(&transaction->t_updates))
		wake_up(&journal->j_wait_updates);

	rwsem_release(&journal->j_trans_commit_map, _THIS_IP_);
	/*
	 * Scope of the GFP_NOFS context is over here and so we can restore the
	 * original alloc context.
	 */
	memalloc_nofs_restore(handle->saved_alloc_context);
}