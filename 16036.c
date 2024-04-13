static void attach_mnt(struct mount *mnt,
			struct mount *parent,
			struct mountpoint *mp)
{
	mnt_set_mountpoint(parent, mp, mnt);
	__attach_mnt(mnt, parent);
}