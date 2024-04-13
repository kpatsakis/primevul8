static int selinux_vm_enough_memory(struct mm_struct *mm, long pages)
{
	int rc, cap_sys_admin = 0;

	rc = selinux_capable(current_cred(), &init_user_ns, CAP_SYS_ADMIN,
			     SECURITY_CAP_NOAUDIT);
	if (rc == 0)
		cap_sys_admin = 1;

	return __vm_enough_memory(mm, pages, cap_sys_admin);
}