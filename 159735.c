static struct bt_att_chan *bt_att_chan_new(int fd, uint8_t type)
{
	struct bt_att_chan *chan;

	if (fd < 0)
		return NULL;

	chan = new0(struct bt_att_chan, 1);
	chan->fd = fd;

	chan->io = io_new(fd);
	if (!chan->io)
		goto fail;

	if (!io_set_read_handler(chan->io, can_read_data, chan, NULL))
		goto fail;

	if (!io_set_disconnect_handler(chan->io, disconnect_cb, chan, NULL))
		goto fail;

	chan->type = type;
	switch (chan->type) {
	case BT_ATT_LOCAL:
		chan->sec_level = BT_ATT_SECURITY_LOW;
		/* fall through */
	case BT_ATT_LE:
		chan->mtu = BT_ATT_DEFAULT_LE_MTU;
		break;
	default:
		chan->mtu = io_get_mtu(chan->fd);
	}

	if (chan->mtu < BT_ATT_DEFAULT_LE_MTU)
		goto fail;

	chan->buf = malloc(chan->mtu);
	if (!chan->buf)
		goto fail;

	chan->queue = queue_new();

	return chan;

fail:
	bt_att_chan_free(chan);

	return NULL;
}