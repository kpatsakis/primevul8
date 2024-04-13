struct bt_att *bt_att_ref(struct bt_att *att)
{
	if (!att)
		return NULL;

	__sync_fetch_and_add(&att->ref_count, 1);

	return att;
}