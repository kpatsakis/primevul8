bool bt_att_unregister_all(struct bt_att *att)
{
	if (!att)
		return false;

	queue_remove_all(att->notify_list, NULL, NULL, destroy_att_notify);
	queue_remove_all(att->disconn_list, NULL, NULL, destroy_att_disconn);

	return true;
}