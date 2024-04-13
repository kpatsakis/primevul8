uint16_t bt_att_get_mtu(struct bt_att *att)
{
	if (!att)
		return 0;

	return att->mtu;
}