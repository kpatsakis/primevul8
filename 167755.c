static struct inode *hugetlbfs_get_inode(struct super_block *sb,
					struct inode *dir,
					umode_t mode, dev_t dev)
{
	struct inode *inode;
	struct resv_map *resv_map;

	resv_map = resv_map_alloc();
	if (!resv_map)
		return NULL;

	inode = new_inode(sb);
	if (inode) {
		struct hugetlbfs_inode_info *info;
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		lockdep_set_class(&inode->i_mapping->i_mmap_rwsem,
				&hugetlbfs_i_mmap_rwsem_key);
		inode->i_mapping->a_ops = &hugetlbfs_aops;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		inode->i_mapping->private_data = resv_map;
		info = HUGETLBFS_I(inode);
		/*
		 * The policy is initialized here even if we are creating a
		 * private inode because initialization simply creates an
		 * an empty rb tree and calls rwlock_init(), later when we
		 * call mpol_free_shared_policy() it will just return because
		 * the rb tree will still be empty.
		 */
		mpol_shared_policy_init(&info->policy, NULL);
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &hugetlbfs_inode_operations;
			inode->i_fop = &hugetlbfs_file_operations;
			break;
		case S_IFDIR:
			inode->i_op = &hugetlbfs_dir_inode_operations;
			inode->i_fop = &simple_dir_operations;

			/* directory inodes start off with i_nlink == 2 (for "." entry) */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(inode);
			break;
		}
		lockdep_annotate_inode_mutex_key(inode);
	} else
		kref_put(&resv_map->refs, resv_map_release);

	return inode;
}