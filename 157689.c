int jbd2_journal_try_to_free_buffers(journal_t *journal, struct page *page)
{
	struct buffer_head *head;
	struct buffer_head *bh;
	int ret = 0;

	J_ASSERT(PageLocked(page));

	head = page_buffers(page);
	bh = head;
	do {
		struct journal_head *jh;

		/*
		 * We take our own ref against the journal_head here to avoid
		 * having to add tons of locking around each instance of
		 * jbd2_journal_put_journal_head().
		 */
		jh = jbd2_journal_grab_journal_head(bh);
		if (!jh)
			continue;

		spin_lock(&jh->b_state_lock);
		__journal_try_to_free_buffer(journal, bh);
		spin_unlock(&jh->b_state_lock);
		jbd2_journal_put_journal_head(jh);
		if (buffer_jbd(bh))
			goto busy;
	} while ((bh = bh->b_this_page) != head);

	ret = try_to_free_buffers(page);
busy:
	return ret;
}