static void process_marithmetic_command(conn *c, token_t *tokens, const size_t ntokens) {
    char *key;
    size_t nkey;
    int i;
    struct _meta_flags of = {0}; // option bitflags.
    char *errstr = "CLIENT_ERROR bad command line format";
    mc_resp *resp = c->resp;
    // no reservation (like del/set) since we post-process the status line.
    char *p = resp->wbuf;

    // If no argument supplied, incr or decr by one.
    of.delta = 1;
    of.initial = 0; // redundant, for clarity.
    bool incr = true; // default mode is to increment.
    bool locked = false;
    uint32_t hv = 0;
    item *it = NULL; // item returned by do_add_delta.

    assert(c != NULL);
    WANT_TOKENS_MIN(ntokens, 3);

    // TODO: most of this is identical to mget.
    if (tokens[KEY_TOKEN].length > KEY_MAX_LENGTH) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }

    key = tokens[KEY_TOKEN].value;
    nkey = tokens[KEY_TOKEN].length;

    if (ntokens > MFLAG_MAX_OPT_LENGTH) {
        out_string(c, "CLIENT_ERROR options flags too long");
        return;
    }

    // scrubs duplicated options and sets flags for how to load the item.
    if (_meta_flag_preparse(tokens, ntokens, &of, &errstr) != 0) {
        out_errstring(c, "CLIENT_ERROR invalid or duplicate flag");
        return;
    }
    c->noreply = of.no_reply;

    assert(c != NULL);
    // "mode switch" to alternative commands
    switch (of.mode) {
        case 0: // no switch supplied.
            break;
        case 'I': // Incr (default)
        case '+':
            incr = true;
            break;
        case 'D': // Decr.
        case '-':
            incr = false;
            break;
        default:
            errstr = "CLIENT_ERROR invalid mode for ma M token";
            goto error;
            break;
    }

    // take hash value and manually lock item... hold lock during store phase
    // on miss and avoid recalculating the hash multiple times.
    hv = hash(key, nkey);
    item_lock(hv);
    locked = true;
    char tmpbuf[INCR_MAX_STORAGE_LEN];

    // return a referenced item if it exists, so we can modify it here, rather
    // than adding even more parameters to do_add_delta.
    bool item_created = false;
    switch(do_add_delta(c, key, nkey, incr, of.delta, tmpbuf, &of.req_cas_id, hv, &it)) {
    case OK:
        if (c->noreply)
            resp->skip = true;
        memcpy(resp->wbuf, "OK ", 3);
        break;
    case NON_NUMERIC:
        errstr = "CLIENT_ERROR cannot increment or decrement non-numeric value";
        goto error;
        break;
    case EOM:
        errstr = "SERVER_ERROR out of memory";
        goto error;
        break;
    case DELTA_ITEM_NOT_FOUND:
        if (of.vivify) {
            itoa_u64(of.initial, tmpbuf);
            int vlen = strlen(tmpbuf);

            it = item_alloc(key, nkey, 0, 0, vlen+2);
            if (it != NULL) {
                memcpy(ITEM_data(it), tmpbuf, vlen);
                memcpy(ITEM_data(it) + vlen, "\r\n", 2);
                if (do_store_item(it, NREAD_ADD, c, hv)) {
                    item_created = true;
                } else {
                    // Not sure how we can get here if we're holding the lock.
                    memcpy(resp->wbuf, "NS ", 3);
                }
            } else {
                errstr = "SERVER_ERROR Out of memory allocating new item";
                goto error;
            }
        } else {
            pthread_mutex_lock(&c->thread->stats.mutex);
            if (incr) {
                c->thread->stats.incr_misses++;
            } else {
                c->thread->stats.decr_misses++;
            }
            pthread_mutex_unlock(&c->thread->stats.mutex);
            // won't have a valid it here.
            memcpy(p, "NF ", 3);
            p += 3;
        }
        break;
    case DELTA_ITEM_CAS_MISMATCH:
        // also returns without a valid it.
        memcpy(p, "EX ", 3);
        p += 3;
        break;
    }

    // final loop
    // allows building the response with information after vivifying from a
    // miss, or returning a new CAS value after add_delta().
    if (it) {
        size_t vlen = strlen(tmpbuf);
        if (of.value) {
            memcpy(p, "VA ", 3);
            p = itoa_u32(vlen, p+3);
        } else {
            memcpy(p, "OK", 2);
            p += 2;
        }

        for (i = KEY_TOKEN+1; i < ntokens-1; i++) {
            switch (tokens[i].value[0]) {
                case 'c':
                    META_CHAR(p, 'c');
                    p = itoa_u64(ITEM_get_cas(it), p);
                    break;
                case 't':
                    META_CHAR(p, 't');
                    if (it->exptime == 0) {
                        *p = '-';
                        *(p+1) = '1';
                        p += 2;
                    } else {
                        p = itoa_u32(it->exptime - current_time, p);
                    }
                    break;
                case 'T':
                    it->exptime = of.exptime;
                    break;
                case 'N':
                    if (item_created) {
                        it->exptime = of.autoviv_exptime;
                    }
                    break;
                // TODO: macro perhaps?
                case 'O':
                    if (tokens[i].length > MFLAG_MAX_OPAQUE_LENGTH) {
                        errstr = "CLIENT_ERROR opaque token too long";
                        goto error;
                    }
                    META_SPACE(p);
                    memcpy(p, tokens[i].value, tokens[i].length);
                    p += tokens[i].length;
                    break;
                case 'k':
                    META_CHAR(p, 'k');
                    memcpy(p, key, nkey);
                    p += nkey;
                    break;
            }
        }

        if (of.value) {
            *p = '\r';
            *(p+1) = '\n';
            p += 2;
            memcpy(p, tmpbuf, vlen);
            p += vlen;
        }

        do_item_remove(it);
    } else {
        // No item to handle. still need to return opaque/key tokens
        for (i = KEY_TOKEN+1; i < ntokens-1; i++) {
            switch (tokens[i].value[0]) {
                // TODO: macro perhaps?
                case 'O':
                    if (tokens[i].length > MFLAG_MAX_OPAQUE_LENGTH) {
                        errstr = "CLIENT_ERROR opaque token too long";
                        goto error;
                    }
                    META_SPACE(p);
                    memcpy(p, tokens[i].value, tokens[i].length);
                    p += tokens[i].length;
                    break;
                case 'k':
                    META_CHAR(p, 'k');
                    memcpy(p, key, nkey);
                    p += nkey;
                    break;
            }
        }
    }

    item_unlock(hv);

    resp->wbytes = p - resp->wbuf;
    memcpy(resp->wbuf + resp->wbytes, "\r\n", 2);
    resp->wbytes += 2;
    resp_add_iov(resp, resp->wbuf, resp->wbytes);
    conn_set_state(c, conn_new_cmd);
    return;
error:
    if (it != NULL)
        do_item_remove(it);
    if (locked)
        item_unlock(hv);
    out_errstring(c, errstr);
}