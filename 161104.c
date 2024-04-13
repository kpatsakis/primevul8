get_system_identitier(char *system_id, size_t size)
{
#if defined(HAVE_SYS_UTSNAME_H)
	struct utsname u;

	uname(&u);
	strncpy(system_id, u.sysname, size-1);
	system_id[size-1] = '\0';
#elif defined(_WIN32) && !defined(__CYGWIN__)
	strncpy(system_id, "Windows", size-1);
	system_id[size-1] = '\0';
#else
#error no way to get the system identifier on your platform.
#endif
}