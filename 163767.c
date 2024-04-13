int http_process_req_stat_post(struct stream_interface *si, struct http_txn *txn, struct channel *req)
{
	struct proxy *px = NULL;
	struct server *sv = NULL;

	char key[LINESIZE];
	int action = ST_ADM_ACTION_NONE;
	int reprocess = 0;

	int total_servers = 0;
	int altered_servers = 0;

	char *first_param, *cur_param, *next_param, *end_params;
	char *st_cur_param = NULL;
	char *st_next_param = NULL;

	first_param = req->buf->p + txn->req.eoh + 2;
	end_params  = first_param + txn->req.body_len;

	cur_param = next_param = end_params;

	if (end_params >= req->buf->data + req->buf->size - global.tune.maxrewrite) {
		/* Prevent buffer overflow */
		si->applet.ctx.stats.st_code = STAT_STATUS_EXCD;
		return 1;
	}
	else if (end_params > req->buf->p + req->buf->i) {
		/* we need more data */
		si->applet.ctx.stats.st_code = STAT_STATUS_NONE;
		return 0;
	}

	*end_params = '\0';

	si->applet.ctx.stats.st_code = STAT_STATUS_NONE;

	/*
	 * Parse the parameters in reverse order to only store the last value.
	 * From the html form, the backend and the action are at the end.
	 */
	while (cur_param > first_param) {
		char *value;
		int poffset, plen;

		cur_param--;
		if ((*cur_param == '&') || (cur_param == first_param)) {
 reprocess_servers:
			/* Parse the key */
			poffset = (cur_param != first_param ? 1 : 0);
			plen = next_param - cur_param + (cur_param == first_param ? 1 : 0);
			if ((plen > 0) && (plen <= sizeof(key))) {
				strncpy(key, cur_param + poffset, plen);
				key[plen - 1] = '\0';
			} else {
				si->applet.ctx.stats.st_code = STAT_STATUS_EXCD;
				goto out;
			}

			/* Parse the value */
			value = key;
			while (*value != '\0' && *value != '=') {
				value++;
			}
			if (*value == '=') {
				/* Ok, a value is found, we can mark the end of the key */
				*value++ = '\0';
			}

			if (!url_decode(key) || !url_decode(value))
				break;

			/* Now we can check the key to see what to do */
			if (!px && (strcmp(key, "b") == 0)) {
				if ((px = findproxy(value, PR_CAP_BE)) == NULL) {
					/* the backend name is unknown or ambiguous (duplicate names) */
					si->applet.ctx.stats.st_code = STAT_STATUS_ERRP;
					goto out;
				}
			}
			else if (!action && (strcmp(key, "action") == 0)) {
				if (strcmp(value, "disable") == 0) {
					action = ST_ADM_ACTION_DISABLE;
				}
				else if (strcmp(value, "enable") == 0) {
					action = ST_ADM_ACTION_ENABLE;
				}
				else if (strcmp(value, "stop") == 0) {
					action = ST_ADM_ACTION_STOP;
				}
				else if (strcmp(value, "start") == 0) {
					action = ST_ADM_ACTION_START;
				}
				else if (strcmp(value, "shutdown") == 0) {
					action = ST_ADM_ACTION_SHUTDOWN;
				}
				else {
					si->applet.ctx.stats.st_code = STAT_STATUS_ERRP;
					goto out;
				}
			}
			else if (strcmp(key, "s") == 0) {
				if (!(px && action)) {
					/*
					 * Indicates that we'll need to reprocess the parameters
					 * as soon as backend and action are known
					 */
					if (!reprocess) {
						st_cur_param  = cur_param;
						st_next_param = next_param;
					}
					reprocess = 1;
				}
				else if ((sv = findserver(px, value)) != NULL) {
					switch (action) {
					case ST_ADM_ACTION_DISABLE:
						if ((px->state != PR_STSTOPPED) && !(sv->state & SRV_MAINTAIN)) {
							/* Not already in maintenance, we can change the server state */
							sv->state |= SRV_MAINTAIN;
							set_server_down(sv);
							altered_servers++;
							total_servers++;
						}
						break;
					case ST_ADM_ACTION_ENABLE:
						if ((px->state != PR_STSTOPPED) && (sv->state & SRV_MAINTAIN)) {
							/* Already in maintenance, we can change the server state */
							set_server_up(sv);
							sv->health = sv->rise;	/* up, but will fall down at first failure */
							altered_servers++;
							total_servers++;
						}
						break;
					case ST_ADM_ACTION_STOP:
					case ST_ADM_ACTION_START:
						if (action == ST_ADM_ACTION_START)
							sv->uweight = sv->iweight;
						else
							sv->uweight = 0;

						if (px->lbprm.algo & BE_LB_PROP_DYN) {
							/* we must take care of not pushing the server to full throttle during slow starts */
							if ((sv->state & SRV_WARMINGUP) && (px->lbprm.algo & BE_LB_PROP_DYN))
								sv->eweight = (BE_WEIGHT_SCALE * (now.tv_sec - sv->last_change) + sv->slowstart - 1) / sv->slowstart;
							else
								sv->eweight = BE_WEIGHT_SCALE;
							sv->eweight *= sv->uweight;
						} else {
							sv->eweight = sv->uweight;
						}

						/* static LB algorithms are a bit harder to update */
						if (px->lbprm.update_server_eweight)
							px->lbprm.update_server_eweight(sv);
						else if (sv->eweight) {
							if (px->lbprm.set_server_status_up)
								px->lbprm.set_server_status_up(sv);
						}
						else {
							if (px->lbprm.set_server_status_down)
								px->lbprm.set_server_status_down(sv);
						}
						altered_servers++;
						total_servers++;
						break;
					case ST_ADM_ACTION_SHUTDOWN:
						if (px->state != PR_STSTOPPED) {
							struct session *sess, *sess_bck;

							list_for_each_entry_safe(sess, sess_bck, &sv->actconns, by_srv)
								if (sess->srv_conn == sv)
									session_shutdown(sess, SN_ERR_KILLED);

							altered_servers++;
							total_servers++;
						}
						break;
					}
				} else {
					/* the server name is unknown or ambiguous (duplicate names) */
					total_servers++;
				}
			}
			if (reprocess && px && action) {
				/* Now, we know the backend and the action chosen by the user.
				 * We can safely restart from the first server parameter
				 * to reprocess them
				 */
				cur_param  = st_cur_param;
				next_param = st_next_param;
				reprocess = 0;
				goto reprocess_servers;
			}

			next_param = cur_param;
		}
	}

	if (total_servers == 0) {
		si->applet.ctx.stats.st_code = STAT_STATUS_NONE;
	}
	else if (altered_servers == 0) {
		si->applet.ctx.stats.st_code = STAT_STATUS_ERRP;
	}
	else if (altered_servers == total_servers) {
		si->applet.ctx.stats.st_code = STAT_STATUS_DONE;
	}
	else {
		si->applet.ctx.stats.st_code = STAT_STATUS_PART;
	}
 out:
	return 1;
}