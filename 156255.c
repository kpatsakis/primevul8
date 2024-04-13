mm_answer_pam_start(int socket, Buffer *m)
{
	char *user;
	
	user = buffer_get_string(m, NULL);

	start_pam(user);

	xfree(user);

	return (0);
}