    void BasicIo::readOrThrow(byte* buf, long rcount, ErrorCode err) {
      const long nread = read(buf, rcount);
      enforce(nread == rcount, err);
      enforce(!error(), err);
    }