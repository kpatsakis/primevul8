accountingDiffTolerated(XML_Parser originParser, int tok, const char *before,
                        const char *after, int source_line,
                        enum XML_Account account) {
  /* Note: We need to check the token type *first* to be sure that
   *       we can even access variable <after>, safely.
   *       E.g. for XML_TOK_NONE <after> may hold an invalid pointer. */
  switch (tok) {
  case XML_TOK_INVALID:
  case XML_TOK_PARTIAL:
  case XML_TOK_PARTIAL_CHAR:
  case XML_TOK_NONE:
    return XML_TRUE;
  }

  if (account == XML_ACCOUNT_NONE)
    return XML_TRUE; /* because these bytes have been accounted for, already */

  unsigned int levelsAwayFromRootParser;
  const XML_Parser rootParser
      = getRootParserOf(originParser, &levelsAwayFromRootParser);
  assert(! rootParser->m_parentParser);

  const int isDirect
      = (account == XML_ACCOUNT_DIRECT) && (originParser == rootParser);
  const ptrdiff_t bytesMore = after - before;

  XmlBigCount *const additionTarget
      = isDirect ? &rootParser->m_accounting.countBytesDirect
                 : &rootParser->m_accounting.countBytesIndirect;

  /* Detect and avoid integer overflow */
  if (*additionTarget > (XmlBigCount)(-1) - (XmlBigCount)bytesMore)
    return XML_FALSE;
  *additionTarget += bytesMore;

  const XmlBigCount countBytesOutput
      = rootParser->m_accounting.countBytesDirect
        + rootParser->m_accounting.countBytesIndirect;
  const float amplificationFactor
      = accountingGetCurrentAmplification(rootParser);
  const XML_Bool tolerated
      = (countBytesOutput < rootParser->m_accounting.activationThresholdBytes)
        || (amplificationFactor
            <= rootParser->m_accounting.maximumAmplificationFactor);

  if (rootParser->m_accounting.debugLevel >= 2) {
    accountingReportStats(rootParser, "");
    accountingReportDiff(rootParser, levelsAwayFromRootParser, before, after,
                         bytesMore, source_line, account);
  }

  return tolerated;
}