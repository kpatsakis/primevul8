int zmq::stream_engine_t::identity_msg (msg_t *msg_)
{
    int rc = msg_->init_size (options.identity_size);
    errno_assert (rc == 0);
    if (options.identity_size > 0)
        memcpy (msg_->data (), options.identity, options.identity_size);
    next_msg = &stream_engine_t::pull_msg_from_session;
    return 0;
}