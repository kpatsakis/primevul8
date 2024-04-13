static void http_server_error(struct session *t, struct stream_interface *si,
			      int err, int finst, int status, const struct chunk *msg)
{
	channel_auto_read(si->ob);
	channel_abort(si->ob);
	channel_auto_close(si->ob);
	channel_erase(si->ob);
	channel_auto_close(si->ib);
	channel_auto_read(si->ib);
	if (status > 0 && msg) {
		t->txn.status = status;
		bo_inject(si->ib, msg->str, msg->len);
	}
	if (!(t->flags & SN_ERR_MASK))
		t->flags |= err;
	if (!(t->flags & SN_FINST_MASK))
		t->flags |= finst;
}