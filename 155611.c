ns__client_setup(ns_client_t *client, ns_clientmgr_t *mgr, bool new) {
	isc_result_t result;

	/*
	 * Caller must be holding the manager lock.
	 *
	 * Note: creating a client does not add the client to the
	 * manager's client list or set the client's manager pointer.
	 * The caller is responsible for that.
	 */

	REQUIRE(NS_CLIENT_VALID(client) || (new &&client != NULL));
	REQUIRE(VALID_MANAGER(mgr) || !new);

	if (new) {
		*client = (ns_client_t){ .magic = 0 };

		get_clientmctx(mgr, &client->mctx);
		clientmgr_attach(mgr, &client->manager);
		ns_server_attach(mgr->sctx, &client->sctx);
		get_clienttask(mgr, &client->task);

		dns_message_create(client->mctx, DNS_MESSAGE_INTENTPARSE,
				   &client->message);

		client->sendbuf = isc_mem_get(client->mctx,
					      NS_CLIENT_SEND_BUFFER_SIZE);
		/*
		 * Set magic earlier than usual because ns_query_init()
		 * and the functions it calls will require it.
		 */
		client->magic = NS_CLIENT_MAGIC;
		result = ns_query_init(client);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	} else {
		ns_clientmgr_t *oldmgr = client->manager;
		ns_server_t *sctx = client->sctx;
		isc_task_t *task = client->task;
		unsigned char *sendbuf = client->sendbuf;
		dns_message_t *message = client->message;
		isc_mem_t *oldmctx = client->mctx;
		ns_query_t query = client->query;

		/*
		 * Retain these values from the existing client, but
		 * zero every thing else.
		 */
		*client = (ns_client_t){ .magic = 0,
					 .mctx = oldmctx,
					 .manager = oldmgr,
					 .sctx = sctx,
					 .task = task,
					 .sendbuf = sendbuf,
					 .message = message,
					 .query = query };
	}

	client->query.attributes &= ~NS_QUERYATTR_ANSWERED;
	client->state = NS_CLIENTSTATE_INACTIVE;
	client->udpsize = 512;
	client->ednsversion = -1;
	dns_name_init(&client->signername, NULL);
	dns_ecs_init(&client->ecs);
	isc_sockaddr_any(&client->formerrcache.addr);
	client->formerrcache.time = 0;
	client->formerrcache.id = 0;
	ISC_LINK_INIT(client, rlink);
	client->rcode_override = -1; /* not set */

	client->magic = NS_CLIENT_MAGIC;

	CTRACE("client_setup");

	return (ISC_R_SUCCESS);

cleanup:
	if (client->sendbuf != NULL) {
		isc_mem_put(client->mctx, client->sendbuf,
			    NS_CLIENT_SEND_BUFFER_SIZE);
	}

	if (client->message != NULL) {
		dns_message_detach(&client->message);
	}

	if (client->task != NULL) {
		isc_task_detach(&client->task);
	}

	if (client->manager != NULL) {
		clientmgr_detach(&client->manager);
	}
	if (client->mctx != NULL) {
		isc_mem_detach(&client->mctx);
	}
	if (client->sctx != NULL) {
		ns_server_detach(&client->sctx);
	}

	return (result);
}