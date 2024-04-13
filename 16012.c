static void do_idmap_mount(const struct mount_kattr *kattr, struct mount *mnt)
{
	struct user_namespace *mnt_userns;

	if (!kattr->mnt_userns)
		return;

	mnt_userns = get_user_ns(kattr->mnt_userns);
	/* Pairs with smp_load_acquire() in mnt_user_ns(). */
	smp_store_release(&mnt->mnt.mnt_userns, mnt_userns);
}