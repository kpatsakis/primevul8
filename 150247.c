static struct ucma_event *ucma_create_uevent(struct ucma_context *ctx,
					     struct rdma_cm_event *event)
{
	struct ucma_event *uevent;

	uevent = kzalloc(sizeof(*uevent), GFP_KERNEL);
	if (!uevent)
		return NULL;

	uevent->ctx = ctx;
	switch (event->event) {
	case RDMA_CM_EVENT_MULTICAST_JOIN:
	case RDMA_CM_EVENT_MULTICAST_ERROR:
		uevent->mc = (struct ucma_multicast *)
			     event->param.ud.private_data;
		uevent->resp.uid = uevent->mc->uid;
		uevent->resp.id = uevent->mc->id;
		break;
	default:
		uevent->resp.uid = ctx->uid;
		uevent->resp.id = ctx->id;
		break;
	}
	uevent->resp.event = event->event;
	uevent->resp.status = event->status;
	if (ctx->cm_id->qp_type == IB_QPT_UD)
		ucma_copy_ud_event(ctx->cm_id->device, &uevent->resp.param.ud,
				   &event->param.ud);
	else
		ucma_copy_conn_event(&uevent->resp.param.conn,
				     &event->param.conn);

	uevent->resp.ece.vendor_id = event->ece.vendor_id;
	uevent->resp.ece.attr_mod = event->ece.attr_mod;
	return uevent;
}