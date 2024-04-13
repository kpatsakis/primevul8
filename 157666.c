int jbd2_journal_extend(handle_t *handle, int nblocks, int revoke_records)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal;
	int result;
	int wanted;

	if (is_handle_aborted(handle))
		return -EROFS;
	journal = transaction->t_journal;

	result = 1;

	read_lock(&journal->j_state_lock);

	/* Don't extend a locked-down transaction! */
	if (transaction->t_state != T_RUNNING) {
		jbd_debug(3, "denied handle %p %d blocks: "
			  "transaction not running\n", handle, nblocks);
		goto error_out;
	}

	nblocks += DIV_ROUND_UP(
			handle->h_revoke_credits_requested + revoke_records,
			journal->j_revoke_records_per_block) -
		DIV_ROUND_UP(
			handle->h_revoke_credits_requested,
			journal->j_revoke_records_per_block);
	spin_lock(&transaction->t_handle_lock);
	wanted = atomic_add_return(nblocks,
				   &transaction->t_outstanding_credits);

	if (wanted > journal->j_max_transaction_buffers) {
		jbd_debug(3, "denied handle %p %d blocks: "
			  "transaction too large\n", handle, nblocks);
		atomic_sub(nblocks, &transaction->t_outstanding_credits);
		goto unlock;
	}

	trace_jbd2_handle_extend(journal->j_fs_dev->bd_dev,
				 transaction->t_tid,
				 handle->h_type, handle->h_line_no,
				 handle->h_total_credits,
				 nblocks);

	handle->h_total_credits += nblocks;
	handle->h_requested_credits += nblocks;
	handle->h_revoke_credits += revoke_records;
	handle->h_revoke_credits_requested += revoke_records;
	result = 0;

	jbd_debug(3, "extended handle %p by %d\n", handle, nblocks);
unlock:
	spin_unlock(&transaction->t_handle_lock);
error_out:
	read_unlock(&journal->j_state_lock);
	return result;
}