static int inode_alloc_security(struct inode *inode)
{
	struct inode_security_struct *isec;
	u32 sid = current_sid();

	isec = kmem_cache_zalloc(sel_inode_cache, GFP_NOFS);
	if (!isec)
		return -ENOMEM;

	mutex_init(&isec->lock);
	INIT_LIST_HEAD(&isec->list);
	isec->inode = inode;
	isec->sid = SECINITSID_UNLABELED;
	isec->sclass = SECCLASS_FILE;
	isec->task_sid = sid;
	inode->i_security = isec;

	return 0;
}