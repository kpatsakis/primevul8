void mnt_release_group_id(struct mount *mnt)
{
	ida_free(&mnt_group_ida, mnt->mnt_group_id);
	mnt->mnt_group_id = 0;
}