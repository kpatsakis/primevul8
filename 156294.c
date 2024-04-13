start_pam(const char *user)
{
	if (sshpam_init(user) == -1)
		fatal("PAM: initialisation failed");
}