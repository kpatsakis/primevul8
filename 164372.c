static int _meta_flag_preparse(token_t *tokens, const size_t ntokens,
        struct _meta_flags *of, char **errstr) {
    unsigned int i;
    int32_t tmp_int;
    uint8_t seen[127] = {0};
    // Start just past the key token. Look at first character of each token.
    for (i = KEY_TOKEN+1; i < ntokens-1; i++) {
        uint8_t o = (uint8_t)tokens[i].value[0];
        // zero out repeat flags so we don't over-parse for return data.
        if (o >= 127 || seen[o] != 0) {
            *errstr = "CLIENT_ERROR duplicate flag";
            return -1;
        }
        seen[o] = 1;
        switch (o) {
            /* Negative exptimes can underflow and end up immortal. realtime() will
               immediately expire values that are greater than REALTIME_MAXDELTA, but less
               than process_started, so lets aim for that. */
            case 'N':
                of->locked = 1;
                of->vivify = 1;
                if (!safe_strtol(tokens[i].value+1, &tmp_int)) {
                    *errstr = "CLIENT_ERROR bad token in command line format";
                    of->has_error = 1;
                } else {
                    of->autoviv_exptime = realtime(EXPTIME_TO_POSITIVE_TIME(tmp_int));
                }
                break;
            case 'T':
                of->locked = 1;
                if (!safe_strtol(tokens[i].value+1, &tmp_int)) {
                    *errstr = "CLIENT_ERROR bad token in command line format";
                    of->has_error = 1;
                } else {
                    of->exptime = realtime(EXPTIME_TO_POSITIVE_TIME(tmp_int));
                    of->new_ttl = true;
                }
                break;
            case 'R':
                of->locked = 1;
                if (!safe_strtol(tokens[i].value+1, &tmp_int)) {
                    *errstr = "CLIENT_ERROR bad token in command line format";
                    of->has_error = 1;
                } else {
                    of->recache_time = realtime(EXPTIME_TO_POSITIVE_TIME(tmp_int));
                }
                break;
            case 'l':
                of->la = 1;
                of->locked = 1; // need locked to delay LRU bump
                break;
            case 'O':
                break;
            case 'k': // known but no special handling
            case 's':
            case 't':
            case 'c':
            case 'f':
                break;
            case 'v':
                of->value = 1;
                break;
            case 'h':
                of->locked = 1; // need locked to delay LRU bump
                break;
            case 'u':
                of->no_update = 1;
                break;
            case 'q':
                of->no_reply = 1;
                break;
            // mset-related.
            case 'F':
                if (!safe_strtoul(tokens[i].value+1, &of->client_flags)) {
                    of->has_error = true;
                }
                break;
            case 'S':
                if (!safe_strtol(tokens[i].value+1, &tmp_int)) {
                    of->has_error = true;
                } else {
                    // Size is adjusted for underflow or overflow once the
                    // \r\n terminator is added.
                    if (tmp_int < 0 || tmp_int > (INT_MAX - 2)) {
                        *errstr = "CLIENT_ERROR invalid length";
                        of->has_error = true;
                    } else {
                        of->value_len = tmp_int + 2; // \r\n
                    }
                }
                break;
            case 'C': // mset, mdelete, marithmetic
                if (!safe_strtoull(tokens[i].value+1, &of->req_cas_id)) {
                    *errstr = "CLIENT_ERROR bad token in command line format";
                    of->has_error = true;
                } else {
                    of->has_cas = true;
                }
                break;
            case 'M': // mset and marithmetic mode switch
                if (tokens[i].length != 2) {
                    *errstr = "CLIENT_ERROR incorrect length for M token";
                    of->has_error = 1;
                } else {
                    of->mode = tokens[i].value[1];
                }
                break;
            case 'J': // marithmetic initial value
                if (!safe_strtoull(tokens[i].value+1, &of->initial)) {
                    *errstr = "CLIENT_ERROR invalid numeric initial value";
                    of->has_error = 1;
                }
                break;
            case 'D': // marithmetic delta value
                if (!safe_strtoull(tokens[i].value+1, &of->delta)) {
                    *errstr = "CLIENT_ERROR invalid numeric delta value";
                    of->has_error = 1;
                }
                break;
            case 'I':
                of->set_stale = 1;
                break;
            default: // unknown flag, bail.
                *errstr = "CLIENT_ERROR invalid flag";
                return -1;
        }
    }

    return of->has_error ? -1 : 0;
}