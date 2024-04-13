void QuotaManager::DidGetGlobalUsageForEviction(
    StorageType type, int64 usage, int64 unlimited_usage) {
  DCHECK_EQ(type, kStorageTypeTemporary);
  DCHECK_GE(usage, unlimited_usage);
  eviction_context_.usage = usage;
  eviction_context_.unlimited_usage = unlimited_usage;
  GetTemporaryGlobalQuota(callback_factory_.
      NewCallback(&QuotaManager::DidGetGlobalQuotaForEviction));
}
