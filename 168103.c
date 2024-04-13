voter_get_sig_by_algorithm(const networkstatus_voter_info_t *voter,
                           digest_algorithm_t alg)
{
  if (!voter->sigs)
    return NULL;
  SMARTLIST_FOREACH(voter->sigs, document_signature_t *, sig,
    if (sig->alg == alg)
      return sig);
  return NULL;
}