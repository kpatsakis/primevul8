  void DidGetHostUsage(const std::string& host,
                       StorageType type,
                       int64 usage) {
    DCHECK_EQ(host_, host);
    DCHECK_EQ(type_, type);
    host_usage_ = usage;
    CheckCompleted();
  }
