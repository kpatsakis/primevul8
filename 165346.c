int zmq::stream_engine_t::process_identity_msg (msg_t *msg_)
{
    if (options.recv_identity) {
        msg_->set_flags (msg_t::identity);
        int rc = session->push_msg (msg_);
        errno_assert (rc == 0);
    }
    else {
        int rc = msg_->close ();
        errno_assert (rc == 0);
        rc = msg_->init ();
        errno_assert (rc == 0);
    }

    if (subscription_required)
        process_msg = &stream_engine_t::write_subscription_msg;
    else
        process_msg = &stream_engine_t::push_msg_to_session;

    return 0;
}