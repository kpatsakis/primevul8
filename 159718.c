static void disconn_handler(void *data, void *user_data)
{
	struct att_disconn *disconn = data;
	int err = PTR_TO_INT(user_data);

	if (disconn->removed)
		return;

	if (disconn->callback)
		disconn->callback(err, disconn->user_data);
}