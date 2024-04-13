int bt_att_get_channels(struct bt_att *att)
{
	if (!att)
		return 0;

	return queue_length(att->chans);
}