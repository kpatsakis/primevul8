static int mnt_make_readonly(struct mount *mnt)
{
	int ret;

	ret = mnt_hold_writers(mnt);
	if (!ret)
		mnt->mnt.mnt_flags |= MNT_READONLY;
	mnt_unhold_writers(mnt);
	return ret;
}