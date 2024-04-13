void kern_unmount_array(struct vfsmount *mnt[], unsigned int num)
{
	unsigned int i;

	for (i = 0; i < num; i++)
		if (mnt[i])
			real_mount(mnt[i])->mnt_ns = NULL;
	synchronize_rcu_expedited();
	for (i = 0; i < num; i++)
		mntput(mnt[i]);
}