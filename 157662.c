int jbd2_journal_get_undo_access(handle_t *handle, struct buffer_head *bh)
{
	int err;
	struct journal_head *jh;
	char *committed_data = NULL;

	if (is_handle_aborted(handle))
		return -EROFS;

	if (jbd2_write_access_granted(handle, bh, true))
		return 0;

	jh = jbd2_journal_add_journal_head(bh);
	JBUFFER_TRACE(jh, "entry");

	/*
	 * Do this first --- it can drop the journal lock, so we want to
	 * make sure that obtaining the committed_data is done
	 * atomically wrt. completion of any outstanding commits.
	 */
	err = do_get_write_access(handle, jh, 1);
	if (err)
		goto out;

repeat:
	if (!jh->b_committed_data)
		committed_data = jbd2_alloc(jh2bh(jh)->b_size,
					    GFP_NOFS|__GFP_NOFAIL);

	spin_lock(&jh->b_state_lock);
	if (!jh->b_committed_data) {
		/* Copy out the current buffer contents into the
		 * preserved, committed copy. */
		JBUFFER_TRACE(jh, "generate b_committed data");
		if (!committed_data) {
			spin_unlock(&jh->b_state_lock);
			goto repeat;
		}

		jh->b_committed_data = committed_data;
		committed_data = NULL;
		memcpy(jh->b_committed_data, bh->b_data, bh->b_size);
	}
	spin_unlock(&jh->b_state_lock);
out:
	jbd2_journal_put_journal_head(jh);
	if (unlikely(committed_data))
		jbd2_free(committed_data, bh->b_size);
	return err;
}