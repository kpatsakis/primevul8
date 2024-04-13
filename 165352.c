int zmq::stream_engine_t::decode_and_push (msg_t *msg_)
{
    zmq_assert (mechanism != NULL);

    if (mechanism->decode (msg_) == -1)
        return -1;
    if (metadata)
        msg_->set_metadata (metadata);
    if (session->push_msg (msg_) == -1) {
        if (errno == EAGAIN)
            process_msg = &stream_engine_t::push_one_then_decode_and_push;
        return -1;
    }
    return 0;
}