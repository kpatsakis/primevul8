dns_message_checksig(dns_message_t *msg, dns_view_t *view) {
	isc_buffer_t b, msgb;

	REQUIRE(DNS_MESSAGE_VALID(msg));

	if (msg->tsigkey == NULL && msg->tsig == NULL && msg->sig0 == NULL)
		return (ISC_R_SUCCESS);

	INSIST(msg->saved.base != NULL);
	isc_buffer_init(&msgb, msg->saved.base, msg->saved.length);
	isc_buffer_add(&msgb, msg->saved.length);
	if (msg->tsigkey != NULL || msg->tsig != NULL) {
#ifdef SKAN_MSG_DEBUG
		dns_message_dumpsig(msg, "dns_message_checksig#1");
#endif
		if (view != NULL)
			return (dns_view_checksig(view, &msgb, msg));
		else
			return (dns_tsig_verify(&msgb, msg, NULL, NULL));
	} else {
		dns_rdata_t rdata = DNS_RDATA_INIT;
		dns_rdata_sig_t sig;
		dns_rdataset_t keyset;
		isc_result_t result;

		result = dns_rdataset_first(msg->sig0);
		INSIST(result == ISC_R_SUCCESS);
		dns_rdataset_current(msg->sig0, &rdata);

		/*
		 * This can occur when the message is a dynamic update, since
		 * the rdata length checking is relaxed.  This should not
		 * happen in a well-formed message, since the SIG(0) is only
		 * looked for in the additional section, and the dynamic update
		 * meta-records are in the prerequisite and update sections.
		 */
		if (rdata.length == 0)
			return (ISC_R_UNEXPECTEDEND);

		result = dns_rdata_tostruct(&rdata, &sig, msg->mctx);
		if (result != ISC_R_SUCCESS)
			return (result);

		dns_rdataset_init(&keyset);
		if (view == NULL)
			return (DNS_R_KEYUNAUTHORIZED);
		result = dns_view_simplefind(view, &sig.signer,
					     dns_rdatatype_key /* SIG(0) */,
					     0, 0, false, &keyset, NULL);

		if (result != ISC_R_SUCCESS) {
			/* XXXBEW Should possibly create a fetch here */
			result = DNS_R_KEYUNAUTHORIZED;
			goto freesig;
		} else if (keyset.trust < dns_trust_secure) {
			/* XXXBEW Should call a validator here */
			result = DNS_R_KEYUNAUTHORIZED;
			goto freesig;
		}
		result = dns_rdataset_first(&keyset);
		INSIST(result == ISC_R_SUCCESS);
		for (;
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&keyset))
		{
			dst_key_t *key = NULL;

			dns_rdata_reset(&rdata);
			dns_rdataset_current(&keyset, &rdata);
			isc_buffer_init(&b, rdata.data, rdata.length);
			isc_buffer_add(&b, rdata.length);

			result = dst_key_fromdns(&sig.signer, rdata.rdclass,
						 &b, view->mctx, &key);
			if (result != ISC_R_SUCCESS)
				continue;
			if (dst_key_alg(key) != sig.algorithm ||
			    dst_key_id(key) != sig.keyid ||
			    !(dst_key_proto(key) == DNS_KEYPROTO_DNSSEC ||
			      dst_key_proto(key) == DNS_KEYPROTO_ANY))
			{
				dst_key_free(&key);
				continue;
			}
			result = dns_dnssec_verifymessage(&msgb, msg, key);
			dst_key_free(&key);
			if (result == ISC_R_SUCCESS)
				break;
		}
		if (result == ISC_R_NOMORE)
			result = DNS_R_KEYUNAUTHORIZED;

 freesig:
		if (dns_rdataset_isassociated(&keyset))
			dns_rdataset_disassociate(&keyset);
		dns_rdata_freestruct(&sig);
		return (result);
	}
}