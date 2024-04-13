free_pam_environment(char **env)
{
	char **envp;

	for (envp = env; *envp; envp++)
		xfree(*envp);
	xfree(env);
}