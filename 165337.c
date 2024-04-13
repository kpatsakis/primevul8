int zmq::stream_engine_t::pull_and_encode (msg_t *msg_)
{
    zmq_assert (mechanism != NULL);

    if (session->pull_msg (msg_) == -1)
        return -1;
    if (mechanism->encode (msg_) == -1)
        return -1;
    return 0;
}