void bt_att_set_enc_key_size(struct bt_att *att, uint8_t enc_size)
{
	if (!att)
		return;

	att->enc_size = enc_size;
}