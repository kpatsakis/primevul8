privsep_challenge_enable(void)
{
#ifdef BSD_AUTH
	extern KbdintDevice mm_bsdauth_device;
#endif
#ifdef USE_PAM
	extern KbdintDevice mm_sshpam_device;
#endif
#ifdef SKEY
	extern KbdintDevice mm_skey_device;
#endif
	int n = 0;

#ifdef BSD_AUTH
	devices[n++] = &mm_bsdauth_device;
#else
#ifdef USE_PAM
	devices[n++] = &mm_sshpam_device;
#endif
#ifdef SKEY
	devices[n++] = &mm_skey_device;
#endif
#endif
}