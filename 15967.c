static void finish_mount_kattr(struct mount_kattr *kattr)
{
	put_user_ns(kattr->mnt_userns);
	kattr->mnt_userns = NULL;
}