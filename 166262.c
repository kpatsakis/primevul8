int ext4_wait_block_bitmap(struct super_block *sb, ext4_group_t block_group,
			   struct buffer_head *bh)
{
	struct ext4_group_desc *desc;

	if (!buffer_new(bh))
		return 0;
	desc = ext4_get_group_desc(sb, block_group, NULL);
	if (!desc)
		return -EFSCORRUPTED;
	wait_on_buffer(bh);
	if (!buffer_uptodate(bh)) {
		ext4_error(sb, "Cannot read block bitmap - "
			   "block_group = %u, block_bitmap = %llu",
			   block_group, (unsigned long long) bh->b_blocknr);
		ext4_mark_group_bitmap_corrupted(sb, block_group,
					EXT4_GROUP_INFO_BBITMAP_CORRUPT);
		return -EIO;
	}
	clear_buffer_new(bh);
	/* Panic or remount fs read-only if block bitmap is invalid */
	return ext4_validate_block_bitmap(sb, desc, block_group, bh);
}