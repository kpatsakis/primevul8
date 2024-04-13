    DataBuf BasicIo::readOrThrow(long rcount, ErrorCode err) {
      DataBuf result = read(rcount);
      enforce(result.size_ == rcount, err);
      enforce(!error(), err);
      return result;
    }