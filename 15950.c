SYSCALL_DEFINE1(oldumount, char __user *, name)
{
	return ksys_umount(name, 0);
}