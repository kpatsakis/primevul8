int zmq::stream_engine_t::pull_msg_from_session (msg_t *msg_)
{
    return session->pull_msg (msg_);
}