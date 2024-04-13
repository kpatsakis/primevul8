_libssh2_kex_exchange(LIBSSH2_SESSION * session, int reexchange,
                      key_exchange_state_t * key_state)
{
    int rc = 0;
    int retcode;

    session->state |= LIBSSH2_STATE_KEX_ACTIVE;

    if (key_state->state == libssh2_NB_state_idle) {
        /* Prevent loop in packet_add() */
        session->state |= LIBSSH2_STATE_EXCHANGING_KEYS;

        if (reexchange) {
            session->kex = NULL;

            if (session->hostkey && session->hostkey->dtor) {
                session->hostkey->dtor(session,
                                       &session->server_hostkey_abstract);
            }
            session->hostkey = NULL;
        }

        key_state->state = libssh2_NB_state_created;
    }

    if (!session->kex || !session->hostkey) {
        if (key_state->state == libssh2_NB_state_created) {
            /* Preserve in case of failure */
            key_state->oldlocal = session->local.kexinit;
            key_state->oldlocal_len = session->local.kexinit_len;

            session->local.kexinit = NULL;

            key_state->state = libssh2_NB_state_sent;
        }

        if (key_state->state == libssh2_NB_state_sent) {
            retcode = kexinit(session);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            } else if (retcode) {
                session->local.kexinit = key_state->oldlocal;
                session->local.kexinit_len = key_state->oldlocal_len;
                key_state->state = libssh2_NB_state_idle;
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;
                return -1;
            }

            key_state->state = libssh2_NB_state_sent1;
        }

        if (key_state->state == libssh2_NB_state_sent1) {
            retcode =
                _libssh2_packet_require(session, SSH_MSG_KEXINIT,
                                        &key_state->data,
                                        &key_state->data_len, 0, NULL, 0,
                                        &key_state->req_state);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            }
            else if (retcode) {
                if (session->local.kexinit) {
                    LIBSSH2_FREE(session, session->local.kexinit);
                }
                session->local.kexinit = key_state->oldlocal;
                session->local.kexinit_len = key_state->oldlocal_len;
                key_state->state = libssh2_NB_state_idle;
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;
                return -1;
            }

            if (session->remote.kexinit) {
                LIBSSH2_FREE(session, session->remote.kexinit);
            }
            session->remote.kexinit = key_state->data;
            session->remote.kexinit_len = key_state->data_len;

            if (kex_agree_methods(session, key_state->data,
                                  key_state->data_len))
                rc = LIBSSH2_ERROR_KEX_FAILURE;

            key_state->state = libssh2_NB_state_sent2;
        }
    } else {
        key_state->state = libssh2_NB_state_sent2;
    }

    if (rc == 0 && session->kex) {
        if (key_state->state == libssh2_NB_state_sent2) {
            retcode = session->kex->exchange_keys(session,
                                                  &key_state->key_state_low);
            if (retcode == LIBSSH2_ERROR_EAGAIN) {
                session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
                return retcode;
            } else if (retcode) {
                rc = _libssh2_error(session, LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE,
                                    "Unrecoverable error exchanging keys");
            }
        }
    }

    /* Done with kexinit buffers */
    if (session->local.kexinit) {
        LIBSSH2_FREE(session, session->local.kexinit);
        session->local.kexinit = NULL;
    }
    if (session->remote.kexinit) {
        LIBSSH2_FREE(session, session->remote.kexinit);
        session->remote.kexinit = NULL;
    }

    session->state &= ~LIBSSH2_STATE_KEX_ACTIVE;
    session->state &= ~LIBSSH2_STATE_EXCHANGING_KEYS;

    key_state->state = libssh2_NB_state_idle;

    return rc;
}