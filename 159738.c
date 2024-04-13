bool bt_att_set_remote_key(struct bt_att *att, uint8_t sign_key[16],
				bt_att_counter_func_t func, void *user_data)
{
	if (!att)
		return false;

	return sign_set_key(&att->remote_sign, sign_key, func, user_data);
}