mm_chall_setup(char **name, char **infotxt, u_int *numprompts,
    char ***prompts, u_int **echo_on)
{
	*name = xstrdup("");
	*infotxt = xstrdup("");
	*numprompts = 1;
	*prompts = xmalloc(*numprompts * sizeof(char *));
	*echo_on = xmalloc(*numprompts * sizeof(u_int));
	(*echo_on)[0] = 0;
}