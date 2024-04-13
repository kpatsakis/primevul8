bool bt_att_unregister(struct bt_att *att, unsigned int id)
{
	struct att_notify *notify;

	if (!att || !id)
		return false;

	notify = queue_remove_if(att->notify_list, match_notify_id,
							UINT_TO_PTR(id));
	if (!notify)
		return false;

	destroy_att_notify(notify);
	return true;
}