void manage_client_side_appsession(struct session *t, const char *buf, int len) {
	struct http_txn *txn = &t->txn;
	appsess *asession = NULL;
	char *sessid_temp = NULL;

	if (len > t->be->appsession_len) {
		len = t->be->appsession_len;
	}

	if (t->be->options2 & PR_O2_AS_REQL) {
		/* request-learn option is enabled : store the sessid in the session for future use */
		if (txn->sessid != NULL) {
			/* free previously allocated memory as we don't need the session id found in the URL anymore */
			pool_free2(apools.sessid, txn->sessid);
		}

		if ((txn->sessid = pool_alloc2(apools.sessid)) == NULL) {
			Alert("Not enough memory process_cli():asession->sessid:malloc().\n");
			send_log(t->be, LOG_ALERT, "Not enough memory process_cli():asession->sessid:malloc().\n");
			return;
		}

		memcpy(txn->sessid, buf, len);
		txn->sessid[len] = 0;
	}

	if ((sessid_temp = pool_alloc2(apools.sessid)) == NULL) {
		Alert("Not enough memory process_cli():asession->sessid:malloc().\n");
		send_log(t->be, LOG_ALERT, "Not enough memory process_cli():asession->sessid:malloc().\n");
		return;
	}

	memcpy(sessid_temp, buf, len);
	sessid_temp[len] = 0;

	asession = appsession_hash_lookup(&(t->be->htbl_proxy), sessid_temp);
	/* free previously allocated memory */
	pool_free2(apools.sessid, sessid_temp);

	if (asession != NULL) {
		asession->expire = tick_add_ifset(now_ms, t->be->timeout.appsession);
		if (!(t->be->options2 & PR_O2_AS_REQL))
			asession->request_count++;

		if (asession->serverid != NULL) {
			struct server *srv = t->be->srv;

			while (srv) {
				if (strcmp(srv->id, asession->serverid) == 0) {
					if ((srv->state & SRV_RUNNING) ||
					    (t->be->options & PR_O_PERSIST) ||
					    (t->flags & SN_FORCE_PRST)) {
						/* we found the server and it's usable */
						txn->flags &= ~TX_CK_MASK;
						txn->flags |= (srv->state & SRV_RUNNING) ? TX_CK_VALID : TX_CK_DOWN;
						t->flags |= SN_DIRECT | SN_ASSIGNED;
						t->target = &srv->obj_type;

						break;
					} else {
						txn->flags &= ~TX_CK_MASK;
						txn->flags |= TX_CK_DOWN;
					}
				}
				srv = srv->next;
			}
		}
	}
}