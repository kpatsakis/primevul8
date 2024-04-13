static void mnt_free_id(struct mount *mnt)
{
	ida_free(&mnt_id_ida, mnt->mnt_id);
}