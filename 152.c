void QuotaManager::DidGetDatabaseLRUOrigin(const GURL& origin) {
  if (origins_in_use_.find(origin) != origins_in_use_.end() ||
      access_notified_origins_.find(origin) != access_notified_origins_.end())
    lru_origin_callback_->Run(GURL());
  else
    lru_origin_callback_->Run(origin);
  access_notified_origins_.clear();
  lru_origin_callback_.reset();
}
