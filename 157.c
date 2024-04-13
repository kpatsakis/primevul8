  void DidGetHostQuota(QuotaStatusCode status,
                       const std::string& host,
                       StorageType type,
                       int64 quota) {
    DCHECK_EQ(host_, host);
    DCHECK_EQ(type_, type);
    quota_status_ = status;
    quota_ = quota;
    CheckCompleted();
  }
