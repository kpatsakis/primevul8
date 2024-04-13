static void set_mount_attributes(struct mount *mnt, unsigned int mnt_flags)
{
	mnt_flags |= mnt->mnt.mnt_flags & ~MNT_USER_SETTABLE_MASK;
	mnt->mnt.mnt_flags = mnt_flags;
	touch_mnt_namespace(mnt->mnt_ns);
}