int jbd2_journal_inode_ranged_wait(handle_t *handle, struct jbd2_inode *jinode,
		loff_t start_byte, loff_t length)
{
	return jbd2_journal_file_inode(handle, jinode, JI_WAIT_DATA,
			start_byte, start_byte + length - 1);
}