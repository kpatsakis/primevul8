static void zap_handler (void *handler)
{
    //  Process ZAP requests forever
    while (true) {
        char *version = s_recv (handler);
        if (!version)
            break;          //  Terminating

        char *sequence = s_recv (handler);
        char *domain = s_recv (handler);
        char *address = s_recv (handler);
        char *identity = s_recv (handler);
        char *mechanism = s_recv (handler);
        uint8_t client_key [32];
        int size = zmq_recv (handler, client_key, 32, 0);
        assert (size == 32);

        char client_key_text [41];
        zmq_z85_encode (client_key_text, client_key, 32);

        assert (streq (version, "1.0"));
        assert (streq (mechanism, "CURVE"));
        assert (streq (identity, "IDENT"));

        s_sendmore (handler, version);
        s_sendmore (handler, sequence);

        if (streq (client_key_text, client_public)) {
            s_sendmore (handler, "200");
            s_sendmore (handler, "OK");
            s_sendmore (handler, "anonymous");
            s_send     (handler, "");
        }
        else {
            s_sendmore (handler, "400");
            s_sendmore (handler, "Invalid client public key");
            s_sendmore (handler, "");
            s_send     (handler, "");
        }
        free (version);
        free (sequence);
        free (domain);
        free (address);
        free (identity);
        free (mechanism);
    }
    zmq_close (handler);
}