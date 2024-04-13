static void jbd2_get_transaction(journal_t *journal,
				transaction_t *transaction)
{
	transaction->t_journal = journal;
	transaction->t_state = T_RUNNING;
	transaction->t_start_time = ktime_get();
	transaction->t_tid = journal->j_transaction_sequence++;
	transaction->t_expires = jiffies + journal->j_commit_interval;
	spin_lock_init(&transaction->t_handle_lock);
	atomic_set(&transaction->t_updates, 0);
	atomic_set(&transaction->t_outstanding_credits,
		   jbd2_descriptor_blocks_per_trans(journal) +
		   atomic_read(&journal->j_reserved_credits));
	atomic_set(&transaction->t_outstanding_revokes, 0);
	atomic_set(&transaction->t_handle_count, 0);
	INIT_LIST_HEAD(&transaction->t_inode_list);
	INIT_LIST_HEAD(&transaction->t_private_list);

	/* Set up the commit timer for the new transaction. */
	journal->j_commit_timer.expires = round_jiffies_up(transaction->t_expires);
	add_timer(&journal->j_commit_timer);

	J_ASSERT(journal->j_running_transaction == NULL);
	journal->j_running_transaction = transaction;
	transaction->t_max_wait = 0;
	transaction->t_start = jiffies;
	transaction->t_requested = 0;
}