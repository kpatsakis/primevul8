static void bt_att_free(struct bt_att *att)
{
	bt_crypto_unref(att->crypto);

	if (att->timeout_destroy)
		att->timeout_destroy(att->timeout_data);

	if (att->debug_destroy)
		att->debug_destroy(att->debug_data);

	free(att->local_sign);
	free(att->remote_sign);

	queue_destroy(att->req_queue, NULL);
	queue_destroy(att->ind_queue, NULL);
	queue_destroy(att->write_queue, NULL);
	queue_destroy(att->notify_list, NULL);
	queue_destroy(att->disconn_list, NULL);
	queue_destroy(att->chans, bt_att_chan_free);

	free(att);
}