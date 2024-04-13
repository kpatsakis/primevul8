  void DidDeleteOriginData(QuotaStatusCode status) {
    DCHECK_GT(remaining_clients_, 0);

    if (status != kQuotaStatusOk)
      ++error_count_;

    if (--remaining_clients_ == 0)
      CallCompleted();
  }
