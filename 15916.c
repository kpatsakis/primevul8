static void free_vfsmnt(struct mount *mnt)
{
	struct user_namespace *mnt_userns;

	mnt_userns = mnt_user_ns(&mnt->mnt);
	if (mnt_userns != &init_user_ns)
		put_user_ns(mnt_userns);
	kfree_const(mnt->mnt_devname);
#ifdef CONFIG_SMP
	free_percpu(mnt->mnt_pcp);
#endif
	kmem_cache_free(mnt_cache, mnt);
}