get_authname(int type)
{
	static char buf[1024];
	switch (type) {
	case SSH_CMSG_AUTH_PASSWORD:
		return "password";
	case SSH_CMSG_AUTH_RSA:
		return "rsa";
	case SSH_CMSG_AUTH_RHOSTS_RSA:
		return "rhosts-rsa";
	case SSH_CMSG_AUTH_RHOSTS:
		return "rhosts";
	case SSH_CMSG_AUTH_TIS:
	case SSH_CMSG_AUTH_TIS_RESPONSE:
		return "challenge-response";
#if defined(KRB4) || defined(KRB5)
	case SSH_CMSG_AUTH_KERBEROS:
		return "kerberos";
#endif
	}
	snprintf(buf, sizeof buf, "bad-auth-msg-%d", type);
	return buf;
}