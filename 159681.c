unsigned int bt_att_register_disconnect(struct bt_att *att,
					bt_att_disconnect_func_t callback,
					void *user_data,
					bt_att_destroy_func_t destroy)
{
	struct att_disconn *disconn;

	if (!att || queue_isempty(att->chans))
		return 0;

	disconn = new0(struct att_disconn, 1);
	disconn->callback = callback;
	disconn->destroy = destroy;
	disconn->user_data = user_data;

	if (att->next_reg_id < 1)
		att->next_reg_id = 1;

	disconn->id = att->next_reg_id++;

	if (!queue_push_tail(att->disconn_list, disconn)) {
		free(disconn);
		return 0;
	}

	return disconn->id;
}