int ext4_should_retry_alloc(struct super_block *sb, int *retries)
{
	if (!ext4_has_free_clusters(EXT4_SB(sb), 1, 0) ||
	    (*retries)++ > 1 ||
	    !EXT4_SB(sb)->s_journal)
		return 0;

	smp_mb();
	if (EXT4_SB(sb)->s_mb_free_pending == 0)
		return 0;

	jbd_debug(1, "%s: retrying operation after ENOSPC\n", sb->s_id);
	jbd2_journal_force_commit_nested(EXT4_SB(sb)->s_journal);
	return 1;
}