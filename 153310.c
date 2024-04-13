msgresetopt(dns_message_t *msg)
{
	if (msg->opt != NULL) {
		if (msg->opt_reserved > 0) {
			dns_message_renderrelease(msg, msg->opt_reserved);
			msg->opt_reserved = 0;
		}
		INSIST(dns_rdataset_isassociated(msg->opt));
		dns_rdataset_disassociate(msg->opt);
		isc_mempool_put(msg->rdspool, msg->opt);
		msg->opt = NULL;
		msg->cc_ok = 0;
		msg->cc_bad = 0;
	}
}