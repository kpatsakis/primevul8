accountingReportStats(XML_Parser originParser, const char *epilog) {
  const XML_Parser rootParser = getRootParserOf(originParser, NULL);
  assert(! rootParser->m_parentParser);

  if (rootParser->m_accounting.debugLevel < 1) {
    return;
  }

  const float amplificationFactor
      = accountingGetCurrentAmplification(rootParser);
  fprintf(stderr,
          "expat: Accounting(%p): Direct " EXPAT_FMT_ULL(
              "10") ", indirect " EXPAT_FMT_ULL("10") ", amplification %8.2f%s",
          (void *)rootParser, rootParser->m_accounting.countBytesDirect,
          rootParser->m_accounting.countBytesIndirect,
          (double)amplificationFactor, epilog);
}