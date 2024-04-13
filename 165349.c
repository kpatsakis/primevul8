int zmq::stream_engine_t::push_msg_to_session (msg_t *msg_)
{
    return session->push_msg (msg_);
}