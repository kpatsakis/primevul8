static void wakeup_chan_writer(void *data, void *user_data)
{
	struct bt_att_chan *chan = data;
	struct bt_att *att = chan->att;

	if (chan->writer_active)
		return;

	/* Set the write handler only if there is anything that can be sent
	 * at all.
	 */
	if (queue_isempty(chan->queue) && queue_isempty(att->write_queue)) {
		if ((chan->pending_req || queue_isempty(att->req_queue)) &&
			(chan->pending_ind || queue_isempty(att->ind_queue)))
			return;
	}

	if (!io_set_write_handler(chan->io, can_write_data, chan,
							write_watch_destroy))
		return;

	chan->writer_active = true;
}