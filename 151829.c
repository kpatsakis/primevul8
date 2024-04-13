static void red_sasl_async_result(RedSASLAuth *auth, RedSaslError err)
{
    red_stream_set_async_error_handler(auth->stream, auth->saved_error_cb);
    auth->result_cb(auth->result_opaque, err);
    red_sasl_auth_free(auth);
}