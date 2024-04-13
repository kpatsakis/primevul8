ns_client_send(ns_client_t *client) {
	isc_result_t result;
	unsigned char *data;
	isc_buffer_t buffer = { .magic = 0 };
	isc_region_t r;
	dns_compress_t cctx;
	bool cleanup_cctx = false;
	unsigned int render_opts;
	unsigned int preferred_glue;
	bool opt_included = false;
	size_t respsize;
	dns_aclenv_t *env;
#ifdef HAVE_DNSTAP
	unsigned char zone[DNS_NAME_MAXWIRE];
	dns_dtmsgtype_t dtmsgtype;
	isc_region_t zr;
#endif /* HAVE_DNSTAP */

	REQUIRE(NS_CLIENT_VALID(client));

	if ((client->query.attributes & NS_QUERYATTR_ANSWERED) != 0) {
		return;
	}

	/*
	 * XXXWPK TODO
	 * Delay the response according to the -T delay option
	 */

	env = ns_interfacemgr_getaclenv(client->manager->interface->mgr);

	CTRACE("send");

	if (client->message->opcode == dns_opcode_query &&
	    (client->attributes & NS_CLIENTATTR_RA) != 0)
	{
		client->message->flags |= DNS_MESSAGEFLAG_RA;
	}

	if ((client->attributes & NS_CLIENTATTR_WANTDNSSEC) != 0) {
		render_opts = 0;
	} else {
		render_opts = DNS_MESSAGERENDER_OMITDNSSEC;
	}

	preferred_glue = 0;
	if (client->view != NULL) {
		if (client->view->preferred_glue == dns_rdatatype_a) {
			preferred_glue = DNS_MESSAGERENDER_PREFER_A;
		} else if (client->view->preferred_glue == dns_rdatatype_aaaa) {
			preferred_glue = DNS_MESSAGERENDER_PREFER_AAAA;
		}
	}
	if (preferred_glue == 0) {
		if (isc_sockaddr_pf(&client->peeraddr) == AF_INET) {
			preferred_glue = DNS_MESSAGERENDER_PREFER_A;
		} else {
			preferred_glue = DNS_MESSAGERENDER_PREFER_AAAA;
		}
	}

	/*
	 * Create an OPT for our reply.
	 */
	if ((client->attributes & NS_CLIENTATTR_WANTOPT) != 0) {
		result = ns_client_addopt(client, client->message,
					  &client->opt);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	}

	client_allocsendbuf(client, &buffer, &data);

	result = dns_compress_init(&cctx, -1, client->mctx);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}
	if (client->peeraddr_valid && client->view != NULL) {
		isc_netaddr_t netaddr;
		dns_name_t *name = NULL;

		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		if (client->message->tsigkey != NULL) {
			name = &client->message->tsigkey->name;
		}

		if (client->view->nocasecompress == NULL ||
		    !dns_acl_allowed(&netaddr, name,
				     client->view->nocasecompress, env))
		{
			dns_compress_setsensitive(&cctx, true);
		}

		if (!client->view->msgcompression) {
			dns_compress_disable(&cctx);
		}
	}
	cleanup_cctx = true;

	result = dns_message_renderbegin(client->message, &cctx, &buffer);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}

	if (client->opt != NULL) {
		result = dns_message_setopt(client->message, client->opt);
		opt_included = true;
		client->opt = NULL;
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	}
	result = dns_message_rendersection(client->message,
					   DNS_SECTION_QUESTION, 0);
	if (result == ISC_R_NOSPACE) {
		client->message->flags |= DNS_MESSAGEFLAG_TC;
		goto renderend;
	}
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}
	/*
	 * Stop after the question if TC was set for rate limiting.
	 */
	if ((client->message->flags & DNS_MESSAGEFLAG_TC) != 0) {
		goto renderend;
	}
	result = dns_message_rendersection(client->message, DNS_SECTION_ANSWER,
					   DNS_MESSAGERENDER_PARTIAL |
						   render_opts);
	if (result == ISC_R_NOSPACE) {
		client->message->flags |= DNS_MESSAGEFLAG_TC;
		goto renderend;
	}
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}
	result = dns_message_rendersection(
		client->message, DNS_SECTION_AUTHORITY,
		DNS_MESSAGERENDER_PARTIAL | render_opts);
	if (result == ISC_R_NOSPACE) {
		client->message->flags |= DNS_MESSAGEFLAG_TC;
		goto renderend;
	}
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}
	result = dns_message_rendersection(client->message,
					   DNS_SECTION_ADDITIONAL,
					   preferred_glue | render_opts);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOSPACE) {
		goto cleanup;
	}
renderend:
	result = dns_message_renderend(client->message);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}

#ifdef HAVE_DNSTAP
	memset(&zr, 0, sizeof(zr));
	if (((client->message->flags & DNS_MESSAGEFLAG_AA) != 0) &&
	    (client->query.authzone != NULL))
	{
		isc_result_t eresult;
		isc_buffer_t b;
		dns_name_t *zo = dns_zone_getorigin(client->query.authzone);

		isc_buffer_init(&b, zone, sizeof(zone));
		dns_compress_setmethods(&cctx, DNS_COMPRESS_NONE);
		eresult = dns_name_towire(zo, &cctx, &b);
		if (eresult == ISC_R_SUCCESS) {
			isc_buffer_usedregion(&b, &zr);
		}
	}

	if (client->message->opcode == dns_opcode_update) {
		dtmsgtype = DNS_DTTYPE_UR;
	} else if ((client->message->flags & DNS_MESSAGEFLAG_RD) != 0) {
		dtmsgtype = DNS_DTTYPE_CR;
	} else {
		dtmsgtype = DNS_DTTYPE_AR;
	}
#endif /* HAVE_DNSTAP */

	if (cleanup_cctx) {
		dns_compress_invalidate(&cctx);
	}

	if (client->sendcb != NULL) {
		client->sendcb(&buffer);
	} else if (TCP_CLIENT(client)) {
		isc_buffer_usedregion(&buffer, &r);
#ifdef HAVE_DNSTAP
		if (client->view != NULL) {
			dns_dt_send(client->view, dtmsgtype, &client->peeraddr,
				    &client->destsockaddr, true, &zr,
				    &client->requesttime, NULL, &buffer);
		}
#endif /* HAVE_DNSTAP */

		respsize = isc_buffer_usedlength(&buffer);

		client_sendpkg(client, &buffer);

		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(client->sctx->tcpoutstats4,
					    ISC_MIN((int)respsize / 16, 256));
			break;
		case AF_INET6:
			isc_stats_increment(client->sctx->tcpoutstats6,
					    ISC_MIN((int)respsize / 16, 256));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	} else {
#ifdef HAVE_DNSTAP
		/*
		 * Log dnstap data first, because client_sendpkg() may
		 * leave client->view set to NULL.
		 */
		if (client->view != NULL) {
			dns_dt_send(client->view, dtmsgtype, &client->peeraddr,
				    &client->destsockaddr, false, &zr,
				    &client->requesttime, NULL, &buffer);
		}
#endif /* HAVE_DNSTAP */

		respsize = isc_buffer_usedlength(&buffer);

		client_sendpkg(client, &buffer);

		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(client->sctx->udpoutstats4,
					    ISC_MIN((int)respsize / 16, 256));
			break;
		case AF_INET6:
			isc_stats_increment(client->sctx->udpoutstats6,
					    ISC_MIN((int)respsize / 16, 256));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}

	/* update statistics (XXXJT: is it okay to access message->xxxkey?) */
	ns_stats_increment(client->sctx->nsstats, ns_statscounter_response);

	dns_rcodestats_increment(client->sctx->rcodestats,
				 client->message->rcode);
	if (opt_included) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_edns0out);
	}
	if (client->message->tsigkey != NULL) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_tsigout);
	}
	if (client->message->sig0key != NULL) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_sig0out);
	}
	if ((client->message->flags & DNS_MESSAGEFLAG_TC) != 0) {
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_truncatedresp);
	}

	client->query.attributes |= NS_QUERYATTR_ANSWERED;

	return;

cleanup:
	if (client->tcpbuf != NULL) {
		isc_mem_put(client->mctx, client->tcpbuf,
			    NS_CLIENT_TCP_BUFFER_SIZE);
		client->tcpbuf = NULL;
	}

	if (cleanup_cctx) {
		dns_compress_invalidate(&cctx);
	}
}