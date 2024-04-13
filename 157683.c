static void __jbd2_journal_unfile_buffer(struct journal_head *jh)
{
	J_ASSERT_JH(jh, jh->b_transaction != NULL);
	J_ASSERT_JH(jh, jh->b_next_transaction == NULL);

	__jbd2_journal_temp_unlink_buffer(jh);
	jh->b_transaction = NULL;
}