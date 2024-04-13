msgresetsigs(dns_message_t *msg, bool replying) {
	if (msg->sig_reserved > 0) {
		dns_message_renderrelease(msg, msg->sig_reserved);
		msg->sig_reserved = 0;
	}
	if (msg->tsig != NULL) {
		INSIST(dns_rdataset_isassociated(msg->tsig));
		INSIST(msg->namepool != NULL);
		if (replying) {
			INSIST(msg->querytsig == NULL);
			msg->querytsig = msg->tsig;
		} else {
			dns_rdataset_disassociate(msg->tsig);
			isc_mempool_put(msg->rdspool, msg->tsig);
			if (msg->querytsig != NULL) {
				dns_rdataset_disassociate(msg->querytsig);
				isc_mempool_put(msg->rdspool, msg->querytsig);
			}
		}
		if (dns_name_dynamic(msg->tsigname))
			dns_name_free(msg->tsigname, msg->mctx);
		isc_mempool_put(msg->namepool, msg->tsigname);
		msg->tsig = NULL;
		msg->tsigname = NULL;
	} else if (msg->querytsig != NULL && !replying) {
		dns_rdataset_disassociate(msg->querytsig);
		isc_mempool_put(msg->rdspool, msg->querytsig);
		msg->querytsig = NULL;
	}
	if (msg->sig0 != NULL) {
		INSIST(dns_rdataset_isassociated(msg->sig0));
		dns_rdataset_disassociate(msg->sig0);
		isc_mempool_put(msg->rdspool, msg->sig0);
		if (msg->sig0name != NULL) {
			if (dns_name_dynamic(msg->sig0name))
				dns_name_free(msg->sig0name, msg->mctx);
			isc_mempool_put(msg->namepool, msg->sig0name);
		}
		msg->sig0 = NULL;
		msg->sig0name = NULL;
	}
}