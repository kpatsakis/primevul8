	__releases(journal->j_state_lock)
{
	DEFINE_WAIT(wait);

	if (WARN_ON(!journal->j_running_transaction ||
		    journal->j_running_transaction->t_state != T_SWITCH)) {
		read_unlock(&journal->j_state_lock);
		return;
	}
	prepare_to_wait(&journal->j_wait_transaction_locked, &wait,
			TASK_UNINTERRUPTIBLE);
	read_unlock(&journal->j_state_lock);
	/*
	 * We don't call jbd2_might_wait_for_commit() here as there's no
	 * waiting for outstanding handles happening anymore in T_SWITCH state
	 * and handling of reserved handles actually relies on that for
	 * correctness.
	 */
	schedule();
	finish_wait(&journal->j_wait_transaction_locked, &wait);
}