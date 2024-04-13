static void red_sasl_handle_auth_mechname(void *opaque)
{
    RedSASLAuth *auth = (RedSASLAuth*) opaque;

    auth->mechname[auth->len] = '\0';
    spice_debug("Got client mechname '%s' check against '%s'",
                auth->mechname, auth->mechlist);

    char quoted_mechname[SASL_MAX_MECHNAME_LEN + 4];
    sprintf(quoted_mechname, ",%s,", auth->mechname);

    if (strchr(auth->mechname, ',') || strstr(auth->mechlist, quoted_mechname) == NULL) {
        return red_sasl_async_result(auth, RED_SASL_ERROR_INVALID_DATA);
    }

    spice_debug("Validated mechname '%s'", auth->mechname);

    red_stream_async_read(auth->stream, (uint8_t *)&auth->len, sizeof(uint32_t),
                          red_sasl_handle_auth_steplen, auth);
}