monitor_reset_key_state(void)
{
	/* reset state */
	if (key_blob != NULL)
		xfree(key_blob);
	if (hostbased_cuser != NULL)
		xfree(hostbased_cuser);
	if (hostbased_chost != NULL)
		xfree(hostbased_chost);
	key_blob = NULL;
	key_bloblen = 0;
	key_blobtype = MM_NOKEY;
	hostbased_cuser = NULL;
	hostbased_chost = NULL;
}