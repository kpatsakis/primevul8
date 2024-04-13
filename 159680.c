bool bt_att_unregister_disconnect(struct bt_att *att, unsigned int id)
{
	struct att_disconn *disconn;

	if (!att || !id)
		return false;

	/* Check if disconnect is running */
	if (queue_isempty(att->chans)) {
		disconn = queue_find(att->disconn_list, match_disconn_id,
							UINT_TO_PTR(id));
		if (!disconn)
			return false;

		disconn->removed = true;
		return true;
	}

	disconn = queue_remove_if(att->disconn_list, match_disconn_id,
							UINT_TO_PTR(id));
	if (!disconn)
		return false;

	destroy_att_disconn(disconn);
	return true;
}