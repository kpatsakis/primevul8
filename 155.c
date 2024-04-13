  void DidGetGlobalUsage(StorageType type, int64 usage,
                         int64 unlimited_usage) {
    DCHECK_EQ(type_, type);
    DCHECK_GE(usage, unlimited_usage);
    global_usage_ = usage;
    global_unlimited_usage_ = unlimited_usage;
    CheckCompleted();
  }
