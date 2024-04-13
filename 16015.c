static void dec_mnt_namespaces(struct ucounts *ucounts)
{
	dec_ucount(ucounts, UCOUNT_MNT_NAMESPACES);
}