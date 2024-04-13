void QuotaManager::DidGetGlobalQuotaForEviction(
    QuotaStatusCode status,
    StorageType type,
    int64 quota) {
  DCHECK_EQ(type, kStorageTypeTemporary);
  if (status != kQuotaStatusOk) {
    eviction_context_.get_usage_and_quota_callback->Run(
        status, 0, 0, 0, 0);
    eviction_context_.get_usage_and_quota_callback.reset();
    return;
  }

  eviction_context_.quota = quota;
  GetAvailableSpace(callback_factory_.
      NewCallback(&QuotaManager::DidGetAvailableSpaceForEviction));
}
