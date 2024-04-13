void bt_att_unref(struct bt_att *att)
{
	if (!att)
		return;

	if (__sync_sub_and_fetch(&att->ref_count, 1))
		return;

	bt_att_unregister_all(att);
	bt_att_cancel_all(att);

	bt_att_free(att);
}