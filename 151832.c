static void red_sasl_handle_auth_step(void *opaque)
{
    RedSASLAuth *auth = (RedSASLAuth*) opaque;
    RedStream *stream = auth->stream;
    const char *serverout;
    unsigned int serveroutlen;
    int err;
    char *clientdata = NULL;
    RedSASL *sasl = &stream->priv->sasl;
    uint32_t datalen = auth->len;

    /* NB, distinction of NULL vs "" is *critical* in SASL */
    if (datalen) {
        clientdata = auth->data;
        clientdata[datalen - 1] = '\0'; /* Wire includes '\0', but make sure */
        datalen--; /* Don't count NULL byte when passing to _start() */
    }

    if (auth->mechname != NULL) {
        spice_debug("Start SASL auth with mechanism %s. Data %p (%d bytes)",
                    auth->mechname, clientdata, datalen);
        err = sasl_server_start(sasl->conn,
                                auth->mechname,
                                clientdata,
                                datalen,
                                &serverout,
                                &serveroutlen);
        g_free(auth->mechname);
        auth->mechname = NULL;
    } else {
        spice_debug("Step using SASL Data %p (%d bytes)", clientdata, datalen);
        err = sasl_server_step(sasl->conn,
                               clientdata,
                               datalen,
                               &serverout,
                               &serveroutlen);
    }
    if (err != SASL_OK &&
        err != SASL_CONTINUE) {
        spice_warning("sasl step failed %d (%s)",
                    err, sasl_errdetail(sasl->conn));
        return red_sasl_async_result(auth, RED_SASL_ERROR_GENERIC);
    }

    if (serveroutlen > SASL_DATA_MAX_LEN) {
        spice_warning("sasl step reply data too long %d",
                      serveroutlen);
        return red_sasl_async_result(auth, RED_SASL_ERROR_GENERIC);
    }

    spice_debug("SASL return data %d bytes, %p", serveroutlen, serverout);

    if (serveroutlen) {
        serveroutlen += 1;
        red_stream_write_u32_le(stream, serveroutlen);
        red_stream_write_all(stream, serverout, serveroutlen);
    } else {
        red_stream_write_u32_le(stream, 0);
    }

    /* Whether auth is complete */
    red_stream_write_u8(stream, err == SASL_CONTINUE ? 0 : 1);

    if (err == SASL_CONTINUE) {
        spice_debug("%s", "Authentication must continue");
        /* Wait for step length */
        red_stream_async_read(stream, (uint8_t *)&auth->len, sizeof(uint32_t),
                              red_sasl_handle_auth_steplen, auth);
        return;
    } else {
        int ssf;

        if (auth_sasl_check_ssf(sasl, &ssf) == 0) {
            spice_warning("Authentication rejected for weak SSF");
            goto authreject;
        }

        spice_debug("Authentication successful");
        red_stream_write_u32_le(stream, SPICE_LINK_ERR_OK); /* Accept auth */

        /*
         * Delay writing in SSF encoded until now
         */
        sasl->runSSF = ssf;
        red_stream_disable_writev(stream); /* make sure writev isn't called directly anymore */

        return red_sasl_async_result(auth, RED_SASL_ERROR_OK);
    }

authreject:
    red_stream_write_u32_le(stream, 1); /* Reject auth */
    red_stream_write_u32_le(stream, sizeof("Authentication failed"));
    red_stream_write_all(stream, "Authentication failed", sizeof("Authentication failed"));

    red_sasl_async_result(auth, RED_SASL_ERROR_AUTH_FAILED);
}