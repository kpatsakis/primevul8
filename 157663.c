static int jbd2_descriptor_blocks_per_trans(journal_t *journal)
{
	int tag_space = journal->j_blocksize - sizeof(journal_header_t);
	int tags_per_block;

	/* Subtract UUID */
	tag_space -= 16;
	if (jbd2_journal_has_csum_v2or3(journal))
		tag_space -= sizeof(struct jbd2_journal_block_tail);
	/* Commit code leaves a slack space of 16 bytes at the end of block */
	tags_per_block = (tag_space - 16) / journal_tag_bytes(journal);
	/*
	 * Revoke descriptors are accounted separately so we need to reserve
	 * space for commit block and normal transaction descriptor blocks.
	 */
	return 1 + DIV_ROUND_UP(journal->j_max_transaction_buffers,
				tags_per_block);
}