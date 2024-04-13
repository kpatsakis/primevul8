static inline bool may_mandlock(void)
{
	pr_warn("VFS: \"mand\" mount option not supported");
	return false;
}