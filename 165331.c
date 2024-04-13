int zmq::stream_engine_t::push_one_then_decode_and_push (msg_t *msg_)
{
    const int rc = session->push_msg (msg_);
    if (rc == 0)
        process_msg = &stream_engine_t::decode_and_push;
    return rc;
}