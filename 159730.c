bool bt_att_has_crypto(struct bt_att *att)
{
	if (!att)
		return false;

	return att->crypto ? true : false;
}