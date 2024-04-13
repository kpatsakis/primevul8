SYSCALL_DEFINE2(umount, char __user *, name, int, flags)
{
	return ksys_umount(name, flags);
}