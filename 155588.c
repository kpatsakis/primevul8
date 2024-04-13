ns_client_logv(ns_client_t *client, isc_logcategory_t *category,
	       isc_logmodule_t *module, int level, const char *fmt,
	       va_list ap) {
	char msgbuf[4096];
	char signerbuf[DNS_NAME_FORMATSIZE], qnamebuf[DNS_NAME_FORMATSIZE];
	char peerbuf[ISC_SOCKADDR_FORMATSIZE];
	const char *viewname = "";
	const char *sep1 = "", *sep2 = "", *sep3 = "", *sep4 = "";
	const char *signer = "", *qname = "";
	dns_name_t *q = NULL;

	REQUIRE(client != NULL);

	vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);

	if (client->signer != NULL) {
		dns_name_format(client->signer, signerbuf, sizeof(signerbuf));
		sep1 = "/key ";
		signer = signerbuf;
	}

	q = client->query.origqname != NULL ? client->query.origqname
					    : client->query.qname;
	if (q != NULL) {
		dns_name_format(q, qnamebuf, sizeof(qnamebuf));
		sep2 = " (";
		sep3 = ")";
		qname = qnamebuf;
	}

	if (client->view != NULL && strcmp(client->view->name, "_bind") != 0 &&
	    strcmp(client->view->name, "_default") != 0)
	{
		sep4 = ": view ";
		viewname = client->view->name;
	}

	if (client->peeraddr_valid) {
		isc_sockaddr_format(&client->peeraddr, peerbuf,
				    sizeof(peerbuf));
	} else {
		snprintf(peerbuf, sizeof(peerbuf), "(no-peer)");
	}

	isc_log_write(ns_lctx, category, module, level,
		      "client @%p %s%s%s%s%s%s%s%s: %s", client, peerbuf, sep1,
		      signer, sep2, qname, sep3, sep4, viewname, msgbuf);
}