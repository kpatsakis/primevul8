static void _get_extstore_cb(void *e, obj_io *io, int ret) {
    // FIXME: assumes success
    io_wrap *wrap = (io_wrap *)io->data;
    mc_resp *resp = wrap->resp;
    conn *c = wrap->c;
    assert(wrap->active == true);
    item *read_it = (item *)io->buf;
    bool miss = false;

    // TODO: How to do counters for hit/misses?
    if (ret < 1) {
        miss = true;
    } else {
        uint32_t crc2;
        uint32_t crc = (uint32_t) read_it->exptime;
        int x;
        // item is chunked, crc the iov's
        if (io->iov != NULL) {
            // first iov is the header, which we don't use beyond crc
            crc2 = crc32c(0, (char *)io->iov[0].iov_base+STORE_OFFSET, io->iov[0].iov_len-STORE_OFFSET);
            // make sure it's not sent. hack :(
            io->iov[0].iov_len = 0;
            for (x = 1; x < io->iovcnt; x++) {
                crc2 = crc32c(crc2, (char *)io->iov[x].iov_base, io->iov[x].iov_len);
            }
        } else {
            crc2 = crc32c(0, (char *)read_it+STORE_OFFSET, io->len-STORE_OFFSET);
        }

        if (crc != crc2) {
            miss = true;
            wrap->badcrc = true;
        }
    }

    if (miss) {
        if (wrap->noreply) {
            // In all GET cases, noreply means we send nothing back.
            resp->skip = true;
        } else {
            // TODO: This should be movable to the worker thread.
            // Convert the binprot response into a miss response.
            // The header requires knowing a bunch of stateful crap, so rather
            // than simply writing out a "new" miss response we mangle what's
            // already there.
            if (c->protocol == binary_prot) {
                protocol_binary_response_header *header =
                    (protocol_binary_response_header *)resp->wbuf;

                // cut the extra nbytes off of the body_len
                uint32_t body_len = ntohl(header->response.bodylen);
                uint8_t hdr_len = header->response.extlen;
                body_len -= resp->iov[wrap->iovec_data].iov_len + hdr_len;
                resp->tosend -= resp->iov[wrap->iovec_data].iov_len + hdr_len;
                header->response.extlen = 0;
                header->response.status = (uint16_t)htons(PROTOCOL_BINARY_RESPONSE_KEY_ENOENT);
                header->response.bodylen = htonl(body_len);

                // truncate the data response.
                resp->iov[wrap->iovec_data].iov_len = 0;
                // wipe the extlen iov... wish it was just a flat buffer.
                resp->iov[wrap->iovec_data-1].iov_len = 0;
                resp->chunked_data_iov = 0;
            } else {
                int i;
                // Meta commands have EN status lines for miss, rather than
                // END as a trailer as per normal ascii.
                if (resp->iov[0].iov_len >= 3
                        && memcmp(resp->iov[0].iov_base, "VA ", 3) == 0) {
                    // TODO: These miss translators should use specific callback
                    // functions attached to the io wrap. This is weird :(
                    resp->iovcnt = 1;
                    resp->iov[0].iov_len = 4;
                    resp->iov[0].iov_base = "EN\r\n";
                    resp->tosend = 4;
                } else {
                    // Wipe the iovecs up through our data injection.
                    // Allows trailers to be returned (END)
                    for (i = 0; i <= wrap->iovec_data; i++) {
                        resp->tosend -= resp->iov[i].iov_len;
                        resp->iov[i].iov_len = 0;
                        resp->iov[i].iov_base = NULL;
                    }
                }
                resp->chunked_total = 0;
                resp->chunked_data_iov = 0;
            }
        }
        wrap->miss = true;
    } else {
        assert(read_it->slabs_clsid != 0);
        // TODO: should always use it instead of ITEM_data to kill more
        // chunked special casing.
        if ((read_it->it_flags & ITEM_CHUNKED) == 0) {
            resp->iov[wrap->iovec_data].iov_base = ITEM_data(read_it);
        }
        wrap->miss = false;
    }

    c->io_wrapleft--;
    wrap->active = false;
    //assert(c->io_wrapleft >= 0);

    // All IO's have returned, lets re-attach this connection to our original
    // thread.
    if (c->io_wrapleft == 0) {
        assert(c->io_queued == true);
        c->io_queued = false;
        redispatch_conn(c);
    }
}