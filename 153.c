  void DidGetGlobalQuota(QuotaStatusCode status,
                         StorageType type,
                         int64 quota) {
    DCHECK_EQ(type_, type);
    quota_status_ = status;
    quota_ = quota;
    CheckCompleted();
  }
