unsigned int bt_att_register(struct bt_att *att, uint8_t opcode,
						bt_att_notify_func_t callback,
						void *user_data,
						bt_att_destroy_func_t destroy)
{
	struct att_notify *notify;

	if (!att || !callback || queue_isempty(att->chans))
		return 0;

	notify = new0(struct att_notify, 1);
	notify->opcode = opcode;
	notify->callback = callback;
	notify->destroy = destroy;
	notify->user_data = user_data;

	if (att->next_reg_id < 1)
		att->next_reg_id = 1;

	notify->id = att->next_reg_id++;

	if (!queue_push_tail(att->notify_list, notify)) {
		free(notify);
		return 0;
	}

	return notify->id;
}