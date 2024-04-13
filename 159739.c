static void destroy_att_disconn(void *data)
{
	struct att_disconn *disconn = data;

	if (disconn->destroy)
		disconn->destroy(disconn->user_data);

	free(disconn);
}