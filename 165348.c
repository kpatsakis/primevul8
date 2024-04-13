int zmq::stream_engine_t::next_handshake_command (msg_t *msg_)
{
    zmq_assert (mechanism != NULL);

    if (mechanism->status () == mechanism_t::ready) {
        mechanism_ready ();
        return pull_and_encode (msg_);
    }
    else
    if (mechanism->status () == mechanism_t::error) {
        errno = EPROTO;
        return -1;
    }
    else {
        const int rc = mechanism->next_handshake_command (msg_);
        if (rc == 0)
            msg_->set_flags (msg_t::command);
        return rc;
    }
}