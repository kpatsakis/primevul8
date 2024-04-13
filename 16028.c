static inline bool mnt_is_cursor(struct mount *mnt)
{
	return mnt->mnt.mnt_flags & MNT_CURSOR;
}