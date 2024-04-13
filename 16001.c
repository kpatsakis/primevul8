struct ns_common *from_mnt_ns(struct mnt_namespace *mnt)
{
	return &mnt->ns;
}