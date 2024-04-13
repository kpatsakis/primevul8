accountingReportDiff(XML_Parser rootParser,
                     unsigned int levelsAwayFromRootParser, const char *before,
                     const char *after, ptrdiff_t bytesMore, int source_line,
                     enum XML_Account account) {
  assert(! rootParser->m_parentParser);

  fprintf(stderr,
          " (+" EXPAT_FMT_PTRDIFF_T("6") " bytes %s|%d, xmlparse.c:%d) %*s\"",
          bytesMore, (account == XML_ACCOUNT_DIRECT) ? "DIR" : "EXP",
          levelsAwayFromRootParser, source_line, 10, "");

  const char ellipis[] = "[..]";
  const size_t ellipsisLength = sizeof(ellipis) /* because compile-time */ - 1;
  const unsigned int contextLength = 10;

  /* Note: Performance is of no concern here */
  const char *walker = before;
  if ((rootParser->m_accounting.debugLevel >= 3)
      || (after - before)
             <= (ptrdiff_t)(contextLength + ellipsisLength + contextLength)) {
    for (; walker < after; walker++) {
      fprintf(stderr, "%s", unsignedCharToPrintable(walker[0]));
    }
  } else {
    for (; walker < before + contextLength; walker++) {
      fprintf(stderr, "%s", unsignedCharToPrintable(walker[0]));
    }
    fprintf(stderr, ellipis);
    walker = after - contextLength;
    for (; walker < after; walker++) {
      fprintf(stderr, "%s", unsignedCharToPrintable(walker[0]));
    }
  }
  fprintf(stderr, "\"\n");
}