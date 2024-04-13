auth_password(Authctxt *authctxt, const char *password)
{
	struct passwd * pw = authctxt->pw;
	int ok = authctxt->valid;
#if !defined(HAVE_OSF_SIA)
	char *encrypted_password;
	char *pw_password;
	char *salt;
# if defined(__hpux) || defined(HAVE_SECUREWARE)
	struct pr_passwd *spw;
# endif /* __hpux || HAVE_SECUREWARE */
# if defined(HAVE_SHADOW_H) && !defined(DISABLE_SHADOW)
	struct spwd *spw;
# endif
# if defined(HAVE_GETPWANAM) && !defined(DISABLE_SHADOW)
	struct passwd_adjunct *spw;
# endif
# ifdef WITH_AIXAUTHENTICATE
	char *authmsg;
	int authsuccess;
	int reenter = 1;
# endif
#endif /* !defined(HAVE_OSF_SIA) */

	/* deny if no user. */
	if (pw == NULL)
		ok = 0;
#ifndef HAVE_CYGWIN
	if (pw && pw->pw_uid == 0 && options.permit_root_login != PERMIT_YES)
		ok = 0;
#endif
	if (*password == '\0' && options.permit_empty_passwd == 0)
		ok = 0;

#if defined(HAVE_OSF_SIA)
	if (!ok)
		return 0;
	return auth_sia_password(authctxt, password);
#else
	if (!ok)
		return 0;
# ifdef KRB5
	if (options.kerberos_authentication == 1) {
		int ret = auth_krb5_password(authctxt, password);
		if (ret == 1 || ret == 0)
			return ret;
		/* Fall back to ordinary passwd authentication. */
	}
# endif
# ifdef HAVE_CYGWIN
	if (is_winnt) {
		HANDLE hToken = cygwin_logon_user(pw, password);

		if (hToken == INVALID_HANDLE_VALUE)
			return 0;
		cygwin_set_impersonation_token(hToken);
		return 1;
	}
# endif
# ifdef WITH_AIXAUTHENTICATE
	authsuccess = (authenticate(pw->pw_name,password,&reenter,&authmsg) == 0);

	if (authsuccess)
	        /* We don't have a pty yet, so just label the line as "ssh" */
	        if (loginsuccess(authctxt->user,
			get_canonical_hostname(options.verify_reverse_mapping),
			"ssh", &aixloginmsg) < 0)
				aixloginmsg = NULL;

	return(authsuccess);
# endif
# ifdef KRB4
	if (options.kerberos_authentication == 1) {
		int ret = auth_krb4_password(authctxt, password);
		if (ret == 1 || ret == 0)
			return ret;
		/* Fall back to ordinary passwd authentication. */
	}
# endif
# ifdef BSD_AUTH
	if (auth_userokay(pw->pw_name, authctxt->style, "auth-ssh",
	    (char *)password) == 0)
		return 0;
	else
		return 1;
# endif
	pw_password = pw->pw_passwd;

	/*
	 * Various interfaces to shadow or protected password data
	 */
# if defined(HAVE_SHADOW_H) && !defined(DISABLE_SHADOW)
	spw = getspnam(pw->pw_name);
	if (spw != NULL)
		pw_password = spw->sp_pwdp;
# endif /* defined(HAVE_SHADOW_H) && !defined(DISABLE_SHADOW) */

# if defined(HAVE_GETPWANAM) && !defined(DISABLE_SHADOW)
	if (issecure() && (spw = getpwanam(pw->pw_name)) != NULL)
		pw_password = spw->pwa_passwd;
# endif /* defined(HAVE_GETPWANAM) && !defined(DISABLE_SHADOW) */

# ifdef HAVE_SECUREWARE
	if ((spw = getprpwnam(pw->pw_name)) != NULL)
		pw_password = spw->ufld.fd_encrypt;
# endif /* HAVE_SECUREWARE */

# if defined(__hpux) && !defined(HAVE_SECUREWARE)
	if (iscomsec() && (spw = getprpwnam(pw->pw_name)) != NULL)
		pw_password = spw->ufld.fd_encrypt;
# endif /* defined(__hpux) && !defined(HAVE_SECUREWARE) */

	/* Check for users with no password. */
	if ((password[0] == '\0') && (pw_password[0] == '\0'))
		return 1;

	if (pw_password[0] != '\0')
		salt = pw_password;
	else
		salt = "xx";

# ifdef HAVE_MD5_PASSWORDS
	if (is_md5_salt(salt))
		encrypted_password = md5_crypt(password, salt);
	else
		encrypted_password = crypt(password, salt);
# else /* HAVE_MD5_PASSWORDS */
#  if defined(__hpux) && !defined(HAVE_SECUREWARE)
	if (iscomsec())
		encrypted_password = bigcrypt(password, salt);
	else
		encrypted_password = crypt(password, salt);
#  else
#   ifdef HAVE_SECUREWARE
	encrypted_password = bigcrypt(password, salt);
#   else
	encrypted_password = crypt(password, salt);
#   endif /* HAVE_SECUREWARE */
#  endif /* __hpux && !defined(HAVE_SECUREWARE) */
# endif /* HAVE_MD5_PASSWORDS */

	/* Authentication is accepted if the encrypted passwords are identical. */
	return (strcmp(encrypted_password, pw_password) == 0);
#endif /* !HAVE_OSF_SIA */
}