finish_pam(void)
{
	fatal_remove_cleanup(sshpam_cleanup, NULL);
	sshpam_cleanup(NULL);
}