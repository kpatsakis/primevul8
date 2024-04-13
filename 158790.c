static void sctp_process_ext_param(struct sctp_association *asoc,
				    union sctp_params param)
{
	struct net *net = sock_net(asoc->base.sk);
	__u16 num_ext = ntohs(param.p->length) - sizeof(sctp_paramhdr_t);
	int i;

	for (i = 0; i < num_ext; i++) {
		switch (param.ext->chunks[i]) {
		case SCTP_CID_FWD_TSN:
			if (net->sctp.prsctp_enable && !asoc->peer.prsctp_capable)
				    asoc->peer.prsctp_capable = 1;
			break;
		case SCTP_CID_AUTH:
			/* if the peer reports AUTH, assume that he
			 * supports AUTH.
			 */
			if (asoc->ep->auth_enable)
				asoc->peer.auth_capable = 1;
			break;
		case SCTP_CID_ASCONF:
		case SCTP_CID_ASCONF_ACK:
			if (net->sctp.addip_enable)
				asoc->peer.asconf_capable = 1;
			break;
		default:
			break;
		}
	}
}