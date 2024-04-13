struct vfsmount *vfs_create_mount(struct fs_context *fc)
{
	struct mount *mnt;

	if (!fc->root)
		return ERR_PTR(-EINVAL);

	mnt = alloc_vfsmnt(fc->source ?: "none");
	if (!mnt)
		return ERR_PTR(-ENOMEM);

	if (fc->sb_flags & SB_KERNMOUNT)
		mnt->mnt.mnt_flags = MNT_INTERNAL;

	atomic_inc(&fc->root->d_sb->s_active);
	mnt->mnt.mnt_sb		= fc->root->d_sb;
	mnt->mnt.mnt_root	= dget(fc->root);
	mnt->mnt_mountpoint	= mnt->mnt.mnt_root;
	mnt->mnt_parent		= mnt;

	lock_mount_hash();
	list_add_tail(&mnt->mnt_instance, &mnt->mnt.mnt_sb->s_mounts);
	unlock_mount_hash();
	return &mnt->mnt;
}