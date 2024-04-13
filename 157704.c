int jbd2_journal_forget(handle_t *handle, struct buffer_head *bh)
{
	transaction_t *transaction = handle->h_transaction;
	journal_t *journal;
	struct journal_head *jh;
	int drop_reserve = 0;
	int err = 0;
	int was_modified = 0;

	if (is_handle_aborted(handle))
		return -EROFS;
	journal = transaction->t_journal;

	BUFFER_TRACE(bh, "entry");

	jh = jbd2_journal_grab_journal_head(bh);
	if (!jh) {
		__bforget(bh);
		return 0;
	}

	spin_lock(&jh->b_state_lock);

	/* Critical error: attempting to delete a bitmap buffer, maybe?
	 * Don't do any jbd operations, and return an error. */
	if (!J_EXPECT_JH(jh, !jh->b_committed_data,
			 "inconsistent data on disk")) {
		err = -EIO;
		goto drop;
	}

	/* keep track of whether or not this transaction modified us */
	was_modified = jh->b_modified;

	/*
	 * The buffer's going from the transaction, we must drop
	 * all references -bzzz
	 */
	jh->b_modified = 0;

	if (jh->b_transaction == transaction) {
		J_ASSERT_JH(jh, !jh->b_frozen_data);

		/* If we are forgetting a buffer which is already part
		 * of this transaction, then we can just drop it from
		 * the transaction immediately. */
		clear_buffer_dirty(bh);
		clear_buffer_jbddirty(bh);

		JBUFFER_TRACE(jh, "belongs to current transaction: unfile");

		/*
		 * we only want to drop a reference if this transaction
		 * modified the buffer
		 */
		if (was_modified)
			drop_reserve = 1;

		/*
		 * We are no longer going to journal this buffer.
		 * However, the commit of this transaction is still
		 * important to the buffer: the delete that we are now
		 * processing might obsolete an old log entry, so by
		 * committing, we can satisfy the buffer's checkpoint.
		 *
		 * So, if we have a checkpoint on the buffer, we should
		 * now refile the buffer on our BJ_Forget list so that
		 * we know to remove the checkpoint after we commit.
		 */

		spin_lock(&journal->j_list_lock);
		if (jh->b_cp_transaction) {
			__jbd2_journal_temp_unlink_buffer(jh);
			__jbd2_journal_file_buffer(jh, transaction, BJ_Forget);
		} else {
			__jbd2_journal_unfile_buffer(jh);
			jbd2_journal_put_journal_head(jh);
		}
		spin_unlock(&journal->j_list_lock);
	} else if (jh->b_transaction) {
		J_ASSERT_JH(jh, (jh->b_transaction ==
				 journal->j_committing_transaction));
		/* However, if the buffer is still owned by a prior
		 * (committing) transaction, we can't drop it yet... */
		JBUFFER_TRACE(jh, "belongs to older transaction");
		/* ... but we CAN drop it from the new transaction through
		 * marking the buffer as freed and set j_next_transaction to
		 * the new transaction, so that not only the commit code
		 * knows it should clear dirty bits when it is done with the
		 * buffer, but also the buffer can be checkpointed only
		 * after the new transaction commits. */

		set_buffer_freed(bh);

		if (!jh->b_next_transaction) {
			spin_lock(&journal->j_list_lock);
			jh->b_next_transaction = transaction;
			spin_unlock(&journal->j_list_lock);
		} else {
			J_ASSERT(jh->b_next_transaction == transaction);

			/*
			 * only drop a reference if this transaction modified
			 * the buffer
			 */
			if (was_modified)
				drop_reserve = 1;
		}
	} else {
		/*
		 * Finally, if the buffer is not belongs to any
		 * transaction, we can just drop it now if it has no
		 * checkpoint.
		 */
		spin_lock(&journal->j_list_lock);
		if (!jh->b_cp_transaction) {
			JBUFFER_TRACE(jh, "belongs to none transaction");
			spin_unlock(&journal->j_list_lock);
			goto drop;
		}

		/*
		 * Otherwise, if the buffer has been written to disk,
		 * it is safe to remove the checkpoint and drop it.
		 */
		if (!buffer_dirty(bh)) {
			__jbd2_journal_remove_checkpoint(jh);
			spin_unlock(&journal->j_list_lock);
			goto drop;
		}

		/*
		 * The buffer is still not written to disk, we should
		 * attach this buffer to current transaction so that the
		 * buffer can be checkpointed only after the current
		 * transaction commits.
		 */
		clear_buffer_dirty(bh);
		__jbd2_journal_file_buffer(jh, transaction, BJ_Forget);
		spin_unlock(&journal->j_list_lock);
	}
drop:
	__brelse(bh);
	spin_unlock(&jh->b_state_lock);
	jbd2_journal_put_journal_head(jh);
	if (drop_reserve) {
		/* no need to reserve log space for this block -bzzz */
		handle->h_total_credits++;
	}
	return err;
}