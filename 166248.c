ext4_read_block_bitmap(struct super_block *sb, ext4_group_t block_group)
{
	struct buffer_head *bh;
	int err;

	bh = ext4_read_block_bitmap_nowait(sb, block_group);
	if (IS_ERR(bh))
		return bh;
	err = ext4_wait_block_bitmap(sb, block_group, bh);
	if (err) {
		put_bh(bh);
		return ERR_PTR(err);
	}
	return bh;
}