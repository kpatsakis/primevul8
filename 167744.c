static int __init init_hugetlbfs_fs(void)
{
	struct hstate *h;
	int error;
	int i;

	if (!hugepages_supported()) {
		pr_info("disabling because there are no supported hugepage sizes\n");
		return -ENOTSUPP;
	}

	error = -ENOMEM;
	hugetlbfs_inode_cachep = kmem_cache_create("hugetlbfs_inode_cache",
					sizeof(struct hugetlbfs_inode_info),
					0, SLAB_ACCOUNT, init_once);
	if (hugetlbfs_inode_cachep == NULL)
		goto out2;

	error = register_filesystem(&hugetlbfs_fs_type);
	if (error)
		goto out;

	i = 0;
	for_each_hstate(h) {
		char buf[50];
		unsigned ps_kb = 1U << (h->order + PAGE_SHIFT - 10);

		snprintf(buf, sizeof(buf), "pagesize=%uK", ps_kb);
		hugetlbfs_vfsmount[i] = kern_mount_data(&hugetlbfs_fs_type,
							buf);

		if (IS_ERR(hugetlbfs_vfsmount[i])) {
			pr_err("Cannot mount internal hugetlbfs for "
				"page size %uK", ps_kb);
			error = PTR_ERR(hugetlbfs_vfsmount[i]);
			hugetlbfs_vfsmount[i] = NULL;
		}
		i++;
	}
	/* Non default hstates are optional */
	if (!IS_ERR_OR_NULL(hugetlbfs_vfsmount[default_hstate_idx]))
		return 0;

 out:
	kmem_cache_destroy(hugetlbfs_inode_cachep);
 out2:
	return error;
}