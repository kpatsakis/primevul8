int zmq::stream_engine_t::write_credential (msg_t *msg_)
{
    zmq_assert (mechanism != NULL);
    zmq_assert (session != NULL);

    const blob_t credential = mechanism->get_user_id ();
    if (credential.size () > 0) {
        msg_t msg;
        int rc = msg.init_size (credential.size ());
        zmq_assert (rc == 0);
        memcpy (msg.data (), credential.data (), credential.size ());
        msg.set_flags (msg_t::credential);
        rc = session->push_msg (&msg);
        if (rc == -1) {
            rc = msg.close ();
            errno_assert (rc == 0);
            return -1;
        }
    }
    process_msg = &stream_engine_t::decode_and_push;
    return decode_and_push (msg_);
}