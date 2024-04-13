static bool sign_set_key(struct sign_info **sign, uint8_t key[16],
				bt_att_counter_func_t func, void *user_data)
{
	if (!(*sign))
		*sign = new0(struct sign_info, 1);

	(*sign)->counter = func;
	(*sign)->user_data = user_data;
	memcpy((*sign)->key, key, 16);

	return true;
}