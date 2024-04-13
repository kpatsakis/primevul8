static int call_usermodehelper_keys(char *path, char **argv, char **envp,
					struct key *session_keyring, int wait)
{
	return call_usermodehelper_fns(path, argv, envp, wait,
				       umh_keys_init, umh_keys_cleanup,
				       key_get(session_keyring));
}