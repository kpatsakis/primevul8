static inline void process_get_command(conn *c, token_t *tokens, size_t ntokens, bool return_cas, bool should_touch) {
    char *key;
    size_t nkey;
    item *it;
    token_t *key_token = &tokens[KEY_TOKEN];
    int32_t exptime_int = 0;
    rel_time_t exptime = 0;
    bool fail_length = false;
    assert(c != NULL);
    mc_resp *resp = c->resp;

    if (should_touch) {
        // For get and touch commands, use first token as exptime
        if (!safe_strtol(tokens[1].value, &exptime_int)) {
            out_string(c, "CLIENT_ERROR invalid exptime argument");
            return;
        }
        key_token++;
        exptime = realtime(EXPTIME_TO_POSITIVE_TIME(exptime_int));
    }

    do {
        while(key_token->length != 0) {
            bool overflow; // not used here.
            key = key_token->value;
            nkey = key_token->length;

            if (nkey > KEY_MAX_LENGTH) {
                fail_length = true;
                goto stop;
            }

            it = limited_get(key, nkey, c, exptime, should_touch, DO_UPDATE, &overflow);
            if (settings.detail_enabled) {
                stats_prefix_record_get(key, nkey, NULL != it);
            }
            if (it) {
                /*
                 * Construct the response. Each hit adds three elements to the
                 * outgoing data list:
                 *   "VALUE "
                 *   key
                 *   " " + flags + " " + data length + "\r\n" + data (with \r\n)
                 */

                {
                  MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
                                        it->nbytes, ITEM_get_cas(it));
                  int nbytes = it->nbytes;;
                  nbytes = it->nbytes;
                  char *p = resp->wbuf;
                  memcpy(p, "VALUE ", 6);
                  p += 6;
                  memcpy(p, ITEM_key(it), it->nkey);
                  p += it->nkey;
                  p += make_ascii_get_suffix(p, it, return_cas, nbytes);
                  resp_add_iov(resp, resp->wbuf, p - resp->wbuf);

#ifdef EXTSTORE
                  if (it->it_flags & ITEM_HDR) {
                      if (_get_extstore(c, it, resp) != 0) {
                          pthread_mutex_lock(&c->thread->stats.mutex);
                          c->thread->stats.get_oom_extstore++;
                          pthread_mutex_unlock(&c->thread->stats.mutex);

                          item_remove(it);
                          goto stop;
                      }
                  } else if ((it->it_flags & ITEM_CHUNKED) == 0) {
                      resp_add_iov(resp, ITEM_data(it), it->nbytes);
                  } else {
                      resp_add_chunked_iov(resp, it, it->nbytes);
                  }
#else
                  if ((it->it_flags & ITEM_CHUNKED) == 0) {
                      resp_add_iov(resp, ITEM_data(it), it->nbytes);
                  } else {
                      resp_add_chunked_iov(resp, it, it->nbytes);
                  }
#endif
                }

                if (settings.verbose > 1) {
                    int ii;
                    fprintf(stderr, ">%d sending key ", c->sfd);
                    for (ii = 0; ii < it->nkey; ++ii) {
                        fprintf(stderr, "%c", key[ii]);
                    }
                    fprintf(stderr, "\n");
                }

                /* item_get() has incremented it->refcount for us */
                pthread_mutex_lock(&c->thread->stats.mutex);
                if (should_touch) {
                    c->thread->stats.touch_cmds++;
                    c->thread->stats.slab_stats[ITEM_clsid(it)].touch_hits++;
                } else {
                    c->thread->stats.lru_hits[it->slabs_clsid]++;
                    c->thread->stats.get_cmds++;
                }
                pthread_mutex_unlock(&c->thread->stats.mutex);
#ifdef EXTSTORE
                /* If ITEM_HDR, an io_wrap owns the reference. */
                if ((it->it_flags & ITEM_HDR) == 0) {
                    resp->item = it;
                }
#else
                resp->item = it;
#endif
            } else {
                pthread_mutex_lock(&c->thread->stats.mutex);
                if (should_touch) {
                    c->thread->stats.touch_cmds++;
                    c->thread->stats.touch_misses++;
                } else {
                    c->thread->stats.get_misses++;
                    c->thread->stats.get_cmds++;
                }
                MEMCACHED_COMMAND_GET(c->sfd, key, nkey, -1, 0);
                pthread_mutex_unlock(&c->thread->stats.mutex);
            }

            key_token++;
            if (key_token->length != 0) {
                if (!resp_start(c)) {
                    goto stop;
                }
                resp = c->resp;
            }
        }

        /*
         * If the command string hasn't been fully processed, get the next set
         * of tokens.
         */
        if (key_token->value != NULL) {
            ntokens = tokenize_command(key_token->value, tokens, MAX_TOKENS);
            key_token = tokens;
            if (!resp_start(c)) {
                goto stop;
            }
            resp = c->resp;
        }
    } while(key_token->value != NULL);
stop:

    if (settings.verbose > 1)
        fprintf(stderr, ">%d END\n", c->sfd);

    /*
        If the loop was terminated because of out-of-memory, it is not
        reliable to add END\r\n to the buffer, because it might not end
        in \r\n. So we send SERVER_ERROR instead.
    */
    if (key_token->value != NULL) {
        // Kill any stacked responses we had.
        conn_release_items(c);
        // Start a new response object for the error message.
        if (!resp_start(c)) {
            // severe out of memory error.
            conn_set_state(c, conn_closing);
            return;
        }
        if (fail_length) {
            out_string(c, "CLIENT_ERROR bad command line format");
        } else {
            out_of_memory(c, "SERVER_ERROR out of memory writing get response");
        }
    } else {
        // Tag the end token onto the most recent response object.
        resp_add_iov(resp, "END\r\n", 5);
        conn_set_state(c, conn_mwrite);
    }
}