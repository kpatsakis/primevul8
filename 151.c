void QuotaManager::DidGetAvailableSpaceForEviction(
    QuotaStatusCode status,
    int64 available_space) {
  eviction_context_.get_usage_and_quota_callback->Run(status,
      eviction_context_.usage,
      eviction_context_.unlimited_usage,
      eviction_context_.quota, available_space);
  eviction_context_.get_usage_and_quota_callback.reset();
}
