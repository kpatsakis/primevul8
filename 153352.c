dns_message_dumpsig(dns_message_t *msg, char *txt1) {
	dns_rdata_t querytsigrdata = DNS_RDATA_INIT;
	dns_rdata_any_tsig_t querytsig;
	isc_result_t result;

	if (msg->tsig != NULL) {
		result = dns_rdataset_first(msg->tsig);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdataset_current(msg->tsig, &querytsigrdata);
		result = dns_rdata_tostruct(&querytsigrdata, &querytsig, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		hexdump(txt1, "TSIG", querytsig.signature,
			querytsig.siglen);
	}

	if (msg->querytsig != NULL) {
		result = dns_rdataset_first(msg->querytsig);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdataset_current(msg->querytsig, &querytsigrdata);
		result = dns_rdata_tostruct(&querytsigrdata, &querytsig, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		hexdump(txt1, "QUERYTSIG", querytsig.signature,
			querytsig.siglen);
	}
}