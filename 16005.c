static int change_mount_ro_state(struct mount *mnt, unsigned int mnt_flags)
{
	bool readonly_request = (mnt_flags & MNT_READONLY);

	if (readonly_request == __mnt_is_readonly(&mnt->mnt))
		return 0;

	if (readonly_request)
		return mnt_make_readonly(mnt);

	mnt->mnt.mnt_flags &= ~MNT_READONLY;
	return 0;
}