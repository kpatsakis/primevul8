ns_detached_signatures_free(ns_detached_signatures_t *s)
{
  if (!s)
    return;
  if (s->signatures) {
    STRMAP_FOREACH(s->signatures, flavor, smartlist_t *, sigs) {
      SMARTLIST_FOREACH(sigs, document_signature_t *, sig,
                        document_signature_free(sig));
      smartlist_free(sigs);
    } STRMAP_FOREACH_END;
    strmap_free(s->signatures, NULL);
    strmap_free(s->digests, tor_free_);
  }

  tor_free(s);
}