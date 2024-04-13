fetch_pam_environment(void)
{
#ifdef HAVE_PAM_GETENVLIST
	debug("PAM: retrieving environment");
	return (pam_getenvlist(sshpam_handle));
#else
	return (NULL);
#endif
}