void zmq::stream_engine_t::out_event ()
{
    zmq_assert (!io_error);

    //  If write buffer is empty, try to read new data from the encoder.
    if (!outsize) {

        //  Even when we stop polling as soon as there is no
        //  data to send, the poller may invoke out_event one
        //  more time due to 'speculative write' optimisation.
        if (unlikely (encoder == NULL)) {
            zmq_assert (handshaking);
            return;
        }

        outpos = NULL;
        outsize = encoder->encode (&outpos, 0);

        while (outsize < out_batch_size) {
            if ((this->*next_msg) (&tx_msg) == -1)
                break;
            encoder->load_msg (&tx_msg);
            unsigned char *bufptr = outpos + outsize;
            size_t n = encoder->encode (&bufptr, out_batch_size - outsize);
            zmq_assert (n > 0);
            if (outpos == NULL)
                outpos = bufptr;
            outsize += n;
        }

        //  If there is no data to send, stop polling for output.
        if (outsize == 0) {
            output_stopped = true;
            reset_pollout (handle);
            return;
        }
    }

    //  If there are any data to write in write buffer, write as much as
    //  possible to the socket. Note that amount of data to write can be
    //  arbitrarily large. However, we assume that underlying TCP layer has
    //  limited transmission buffer and thus the actual number of bytes
    //  written should be reasonably modest.
    const int nbytes = tcp_write (s, outpos, outsize);

    //  IO error has occurred. We stop waiting for output events.
    //  The engine is not terminated until we detect input error;
    //  this is necessary to prevent losing incoming messages.
    if (nbytes == -1) {
        reset_pollout (handle);
        return;
    }

    outpos += nbytes;
    outsize -= nbytes;

    //  If we are still handshaking and there are no data
    //  to send, stop polling for output.
    if (unlikely (handshaking))
        if (outsize == 0)
            reset_pollout (handle);
}