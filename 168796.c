static BOOL rdg_tunnel_connect(rdpRdg* rdg)
{
	BOOL status;
	wStream* s;
	rdg_send_handshake(rdg);

	while (rdg->state < RDG_CLIENT_STATE_OPENED)
	{
		status = FALSE;
		s = rdg_receive_packet(rdg);

		if (s)
		{
			status = rdg_process_packet(rdg, s);
			Stream_Free(s, TRUE);
		}

		if (!status)
		{
			rdg->context->rdp->transport->layer = TRANSPORT_LAYER_CLOSED;
			return FALSE;
		}
	}

	return TRUE;
}