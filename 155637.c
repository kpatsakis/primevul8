ns__client_request(isc_nmhandle_t *handle, isc_result_t eresult,
		   isc_region_t *region, void *arg) {
	ns_client_t *client = NULL;
	isc_result_t result;
	isc_result_t sigresult = ISC_R_SUCCESS;
	isc_buffer_t *buffer = NULL;
	isc_buffer_t tbuffer;
	dns_rdataset_t *opt = NULL;
	const dns_name_t *signame = NULL;
	bool ra; /* Recursion available. */
	isc_netaddr_t netaddr;
	int match;
	dns_messageid_t id;
	unsigned int flags;
	bool notimp;
	size_t reqsize;
	dns_aclenv_t *env = NULL;
#ifdef HAVE_DNSTAP
	dns_dtmsgtype_t dtmsgtype;
#endif /* ifdef HAVE_DNSTAP */

	if (eresult != ISC_R_SUCCESS) {
		return;
	}

	client = isc_nmhandle_getdata(handle);
	if (client == NULL) {
		ns_interface_t *ifp = (ns_interface_t *)arg;

		INSIST(VALID_MANAGER(ifp->clientmgr));

		client = isc_nmhandle_getextra(handle);

		result = ns__client_setup(client, ifp->clientmgr, true);
		if (result != ISC_R_SUCCESS) {
			return;
		}

		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "allocate new client");
	} else {
		result = ns__client_setup(client, NULL, false);
		if (result != ISC_R_SUCCESS) {
			return;
		}
	}

	client->state = NS_CLIENTSTATE_READY;

	isc_task_pause(client->task);
	if (client->handle == NULL) {
		isc_nmhandle_setdata(handle, client, ns__client_reset_cb,
				     ns__client_put_cb);
		client->handle = handle;
	}

	if (isc_nmhandle_is_stream(handle)) {
		client->attributes |= NS_CLIENTATTR_TCP;
	}

	INSIST(client->recursionquota == NULL);

	INSIST(client->state == NS_CLIENTSTATE_READY);

	(void)atomic_fetch_add_relaxed(&ns_client_requests, 1);

	isc_buffer_init(&tbuffer, region->base, region->length);
	isc_buffer_add(&tbuffer, region->length);
	buffer = &tbuffer;

	client->peeraddr = isc_nmhandle_peeraddr(handle);
	client->peeraddr_valid = true;

	reqsize = isc_buffer_usedlength(buffer);

	client->state = NS_CLIENTSTATE_WORKING;

	TIME_NOW(&client->requesttime);
	client->tnow = client->requesttime;
	client->now = isc_time_seconds(&client->tnow);

	isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);

#if NS_CLIENT_DROPPORT
	if (ns_client_dropport(isc_sockaddr_getport(&client->peeraddr)) ==
	    DROPPORT_REQUEST)
	{
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(10),
			      "dropped request: suspicious port");
		isc_task_unpause(client->task);
		return;
	}
#endif /* if NS_CLIENT_DROPPORT */

	env = ns_interfacemgr_getaclenv(client->manager->interface->mgr);
	if (client->sctx->blackholeacl != NULL &&
	    (dns_acl_match(&netaddr, NULL, client->sctx->blackholeacl, env,
			   &match, NULL) == ISC_R_SUCCESS) &&
	    match > 0)
	{
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(10),
			      "dropped request: blackholed peer");
		isc_task_unpause(client->task);
		return;
	}

	ns_client_log(client, NS_LOGCATEGORY_CLIENT, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), "%s request",
		      TCP_CLIENT(client) ? "TCP" : "UDP");

	result = dns_message_peekheader(buffer, &id, &flags);
	if (result != ISC_R_SUCCESS) {
		/*
		 * There isn't enough header to determine whether
		 * this was a request or a response.  Drop it.
		 */
		isc_task_unpause(client->task);
		return;
	}

	/*
	 * The client object handles requests, not responses.
	 * If this is a UDP response, forward it to the dispatcher.
	 * If it's a TCP response, discard it here.
	 */
	if ((flags & DNS_MESSAGEFLAG_QR) != 0) {
		CTRACE("unexpected response");
		isc_task_unpause(client->task);
		return;
	}

	/*
	 * Update some statistics counters.  Don't count responses.
	 */
	if (isc_sockaddr_pf(&client->peeraddr) == PF_INET) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_requestv4);
	} else {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_requestv6);
	}
	if (TCP_CLIENT(client)) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_requesttcp);
		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(client->sctx->tcpinstats4,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		case AF_INET6:
			isc_stats_increment(client->sctx->tcpinstats6,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	} else {
		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(client->sctx->udpinstats4,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		case AF_INET6:
			isc_stats_increment(client->sctx->udpinstats6,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}

	/*
	 * It's a request.  Parse it.
	 */
	result = dns_message_parse(client->message, buffer, 0);
	if (result != ISC_R_SUCCESS) {
		/*
		 * Parsing the request failed.  Send a response
		 * (typically FORMERR or SERVFAIL).
		 */
		if (result == DNS_R_OPTERR) {
			(void)ns_client_addopt(client, client->message,
					       &client->opt);
		}

		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "message parsing failed: %s",
			      isc_result_totext(result));
		if (result == ISC_R_NOSPACE || result == DNS_R_BADTSIG) {
			result = DNS_R_FORMERR;
		}
		ns_client_error(client, result);
		isc_task_unpause(client->task);
		return;
	}

	/*
	 * Disable pipelined TCP query processing if necessary.
	 */
	if (TCP_CLIENT(client) &&
	    (client->message->opcode != dns_opcode_query ||
	     (client->sctx->keepresporder != NULL &&
	      dns_acl_allowed(&netaddr, NULL, client->sctx->keepresporder,
			      env))))
	{
		isc_nm_tcpdns_sequential(handle);
	}

	dns_opcodestats_increment(client->sctx->opcodestats,
				  client->message->opcode);
	switch (client->message->opcode) {
	case dns_opcode_query:
	case dns_opcode_update:
	case dns_opcode_notify:
		notimp = false;
		break;
	case dns_opcode_iquery:
	default:
		notimp = true;
		break;
	}

	client->message->rcode = dns_rcode_noerror;

	/*
	 * Deal with EDNS.
	 */
	if ((client->sctx->options & NS_SERVER_NOEDNS) != 0) {
		opt = NULL;
	} else {
		opt = dns_message_getopt(client->message);
	}

	client->ecs.source = 0;
	client->ecs.scope = 0;

	if (opt != NULL) {
		/*
		 * Are returning FORMERR to all EDNS queries?
		 * Simulate a STD13 compliant server.
		 */
		if ((client->sctx->options & NS_SERVER_EDNSFORMERR) != 0) {
			ns_client_error(client, DNS_R_FORMERR);
			isc_task_unpause(client->task);
			return;
		}

		/*
		 * Are returning NOTIMP to all EDNS queries?
		 */
		if ((client->sctx->options & NS_SERVER_EDNSNOTIMP) != 0) {
			ns_client_error(client, DNS_R_NOTIMP);
			isc_task_unpause(client->task);
			return;
		}

		/*
		 * Are returning REFUSED to all EDNS queries?
		 */
		if ((client->sctx->options & NS_SERVER_EDNSREFUSED) != 0) {
			ns_client_error(client, DNS_R_REFUSED);
			isc_task_unpause(client->task);
			return;
		}

		/*
		 * Are we dropping all EDNS queries?
		 */
		if ((client->sctx->options & NS_SERVER_DROPEDNS) != 0) {
			ns_client_drop(client, ISC_R_SUCCESS);
			isc_task_unpause(client->task);
			return;
		}

		result = process_opt(client, opt);
		if (result != ISC_R_SUCCESS) {
			isc_task_unpause(client->task);
			return;
		}
	}

	if (client->message->rdclass == 0) {
		if ((client->attributes & NS_CLIENTATTR_WANTCOOKIE) != 0 &&
		    client->message->opcode == dns_opcode_query &&
		    client->message->counts[DNS_SECTION_QUESTION] == 0U)
		{
			result = dns_message_reply(client->message, true);
			if (result != ISC_R_SUCCESS) {
				ns_client_error(client, result);
				isc_task_unpause(client->task);
				return;
			}

			if (notimp) {
				client->message->rcode = dns_rcode_notimp;
			}

			ns_client_send(client);
			isc_task_unpause(client->task);
			return;
		}

		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "message class could not be determined");
		ns_client_dumpmessage(client, "message class could not be "
					      "determined");
		ns_client_error(client, notimp ? DNS_R_NOTIMP : DNS_R_FORMERR);
		isc_task_unpause(client->task);
		return;
	}

	/*
	 * Determine the destination address.  If the receiving interface is
	 * bound to a specific address, we simply use it regardless of the
	 * address family.  All IPv4 queries should fall into this case.
	 * Otherwise, if this is a TCP query, get the address from the
	 * receiving socket (this needs a system call and can be heavy).
	 * For IPv6 UDP queries, we get this from the pktinfo structure (if
	 * supported).
	 *
	 * If all the attempts fail (this can happen due to memory shortage,
	 * etc), we regard this as an error for safety.
	 */
	if ((client->manager->interface->flags & NS_INTERFACEFLAG_ANYADDR) == 0)
	{
		isc_netaddr_fromsockaddr(&client->destaddr,
					 &client->manager->interface->addr);
	} else {
		isc_sockaddr_t sockaddr = isc_nmhandle_localaddr(handle);
		isc_netaddr_fromsockaddr(&client->destaddr, &sockaddr);
	}

	isc_sockaddr_fromnetaddr(&client->destsockaddr, &client->destaddr, 0);

	result = client->sctx->matchingview(&netaddr, &client->destaddr,
					    client->message, env, &sigresult,
					    &client->view);
	if (result != ISC_R_SUCCESS) {
		char classname[DNS_RDATACLASS_FORMATSIZE];

		/*
		 * Do a dummy TSIG verification attempt so that the
		 * response will have a TSIG if the query did, as
		 * required by RFC2845.
		 */
		isc_buffer_t b;
		isc_region_t *r;

		dns_message_resetsig(client->message);

		r = dns_message_getrawmessage(client->message);
		isc_buffer_init(&b, r->base, r->length);
		isc_buffer_add(&b, r->length);
		(void)dns_tsig_verify(&b, client->message, NULL, NULL);

		dns_rdataclass_format(client->message->rdclass, classname,
				      sizeof(classname));
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "no matching view in class '%s'", classname);
		ns_client_dumpmessage(client, "no matching view in class");
		ns_client_error(client, notimp ? DNS_R_NOTIMP : DNS_R_REFUSED);
		isc_task_unpause(client->task);
		return;
	}

	ns_client_log(client, NS_LOGCATEGORY_CLIENT, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(5), "using view '%s'", client->view->name);

	/*
	 * Check for a signature.  We log bad signatures regardless of
	 * whether they ultimately cause the request to be rejected or
	 * not.  We do not log the lack of a signature unless we are
	 * debugging.
	 */
	client->signer = NULL;
	dns_name_init(&client->signername, NULL);
	result = dns_message_signer(client->message, &client->signername);
	if (result != ISC_R_NOTFOUND) {
		signame = NULL;
		if (dns_message_gettsig(client->message, &signame) != NULL) {
			ns_stats_increment(client->sctx->nsstats,
					   ns_statscounter_tsigin);
		} else {
			ns_stats_increment(client->sctx->nsstats,
					   ns_statscounter_sig0in);
		}
	}
	if (result == ISC_R_SUCCESS) {
		char namebuf[DNS_NAME_FORMATSIZE];
		dns_name_format(&client->signername, namebuf, sizeof(namebuf));
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request has valid signature: %s", namebuf);
		client->signer = &client->signername;
	} else if (result == ISC_R_NOTFOUND) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request is not signed");
	} else if (result == DNS_R_NOIDENTITY) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request is signed by a nonauthoritative key");
	} else {
		char tsigrcode[64];
		isc_buffer_t b;
		dns_rcode_t status;
		isc_result_t tresult;

		/* There is a signature, but it is bad. */
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_invalidsig);
		signame = NULL;
		if (dns_message_gettsig(client->message, &signame) != NULL) {
			char namebuf[DNS_NAME_FORMATSIZE];
			char cnamebuf[DNS_NAME_FORMATSIZE];
			dns_name_format(signame, namebuf, sizeof(namebuf));
			status = client->message->tsigstatus;
			isc_buffer_init(&b, tsigrcode, sizeof(tsigrcode) - 1);
			tresult = dns_tsigrcode_totext(status, &b);
			INSIST(tresult == ISC_R_SUCCESS);
			tsigrcode[isc_buffer_usedlength(&b)] = '\0';
			if (client->message->tsigkey->generated) {
				dns_name_format(
					client->message->tsigkey->creator,
					cnamebuf, sizeof(cnamebuf));
				ns_client_log(
					client, DNS_LOGCATEGORY_SECURITY,
					NS_LOGMODULE_CLIENT, ISC_LOG_ERROR,
					"request has invalid signature: "
					"TSIG %s (%s): %s (%s)",
					namebuf, cnamebuf,
					isc_result_totext(result), tsigrcode);
			} else {
				ns_client_log(
					client, DNS_LOGCATEGORY_SECURITY,
					NS_LOGMODULE_CLIENT, ISC_LOG_ERROR,
					"request has invalid signature: "
					"TSIG %s: %s (%s)",
					namebuf, isc_result_totext(result),
					tsigrcode);
			}
		} else {
			status = client->message->sig0status;
			isc_buffer_init(&b, tsigrcode, sizeof(tsigrcode) - 1);
			tresult = dns_tsigrcode_totext(status, &b);
			INSIST(tresult == ISC_R_SUCCESS);
			tsigrcode[isc_buffer_usedlength(&b)] = '\0';
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_CLIENT, ISC_LOG_ERROR,
				      "request has invalid signature: %s (%s)",
				      isc_result_totext(result), tsigrcode);
		}

		/*
		 * Accept update messages signed by unknown keys so that
		 * update forwarding works transparently through slaves
		 * that don't have all the same keys as the master.
		 */
		if (!(client->message->tsigstatus == dns_tsigerror_badkey &&
		      client->message->opcode == dns_opcode_update))
		{
			ns_client_error(client, sigresult);
			isc_task_unpause(client->task);
			return;
		}
	}

	/*
	 * Decide whether recursive service is available to this client.
	 * We do this here rather than in the query code so that we can
	 * set the RA bit correctly on all kinds of responses, not just
	 * responses to ordinary queries.  Note if you can't query the
	 * cache there is no point in setting RA.
	 */
	ra = false;
	if (client->view->resolver != NULL && client->view->recursion &&
	    ns_client_checkaclsilent(client, NULL, client->view->recursionacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, NULL, client->view->cacheacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, &client->destaddr,
				     client->view->recursiononacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, &client->destaddr,
				     client->view->cacheonacl,
				     true) == ISC_R_SUCCESS)
	{
		ra = true;
	}

	if (ra) {
		client->attributes |= NS_CLIENTATTR_RA;
	}

	ns_client_log(client, DNS_LOGCATEGORY_SECURITY, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3),
		      ra ? "recursion available" : "recursion not available");

	/*
	 * Adjust maximum UDP response size for this client.
	 */
	if (client->udpsize > 512) {
		dns_peer_t *peer = NULL;
		uint16_t udpsize = client->view->maxudp;
		(void)dns_peerlist_peerbyaddr(client->view->peers, &netaddr,
					      &peer);
		if (peer != NULL) {
			dns_peer_getmaxudp(peer, &udpsize);
		}
		if (client->udpsize > udpsize) {
			client->udpsize = udpsize;
		}
	}

	/*
	 * Dispatch the request.
	 */
	switch (client->message->opcode) {
	case dns_opcode_query:
		CTRACE("query");
#ifdef HAVE_DNSTAP
		if (ra && (client->message->flags & DNS_MESSAGEFLAG_RD) != 0) {
			dtmsgtype = DNS_DTTYPE_CQ;
		} else {
			dtmsgtype = DNS_DTTYPE_AQ;
		}

		dns_dt_send(client->view, dtmsgtype, &client->peeraddr,
			    &client->destsockaddr, TCP_CLIENT(client), NULL,
			    &client->requesttime, NULL, buffer);
#endif /* HAVE_DNSTAP */

		ns_query_start(client, handle);
		break;
	case dns_opcode_update:
		CTRACE("update");
#ifdef HAVE_DNSTAP
		dns_dt_send(client->view, DNS_DTTYPE_UQ, &client->peeraddr,
			    &client->destsockaddr, TCP_CLIENT(client), NULL,
			    &client->requesttime, NULL, buffer);
#endif /* HAVE_DNSTAP */
		ns_client_settimeout(client, 60);
		ns_update_start(client, handle, sigresult);
		break;
	case dns_opcode_notify:
		CTRACE("notify");
		ns_client_settimeout(client, 60);
		ns_notify_start(client, handle);
		break;
	case dns_opcode_iquery:
		CTRACE("iquery");
		ns_client_error(client, DNS_R_NOTIMP);
		break;
	default:
		CTRACE("unknown opcode");
		ns_client_error(client, DNS_R_NOTIMP);
	}

	isc_task_unpause(client->task);
}