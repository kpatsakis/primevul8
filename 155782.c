poolCopyStringN(STRING_POOL *pool, const XML_Char *s, int n) {
  if (! pool->ptr && ! poolGrow(pool)) {
    /* The following line is unreachable given the current usage of
     * poolCopyStringN().  Currently it is called from exactly one
     * place to copy the text of a simple general entity.  By that
     * point, the name of the entity is already stored in the pool, so
     * pool->ptr cannot be NULL.
     *
     * If poolCopyStringN() is used elsewhere as it well might be,
     * this line may well become executable again.  Regardless, this
     * sort of check shouldn't be removed lightly, so we just exclude
     * it from the coverage statistics.
     */
    return NULL; /* LCOV_EXCL_LINE */
  }
  for (; n > 0; --n, s++) {
    if (! poolAppendChar(pool, *s))
      return NULL;
  }
  s = pool->start;
  poolFinish(pool);
  return s;
}