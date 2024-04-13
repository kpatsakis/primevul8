    void append(Blob& blob, const byte* buf, uint32_t len)
    {
        if (len != 0) {
            assert(buf != 0);
            Blob::size_type size = blob.size();
            if (blob.capacity() - size < len) {
                blob.reserve(size + 65536);
            }
            blob.resize(size + len);
            std::memcpy(&blob[size], buf, len);
        }
    } // append