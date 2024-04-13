consensus_method_is_supported(int method)
{
  if (method == MIN_METHOD_FOR_ED25519_ID_IN_MD) {
    /* This method was broken due to buggy code accidently left in
     * dircollate.c; do not actually use it.
     */
    return 0;
  }

  return (method >= MIN_SUPPORTED_CONSENSUS_METHOD) &&
    (method <= MAX_SUPPORTED_CONSENSUS_METHOD);
}