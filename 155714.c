XML_SetBillionLaughsAttackProtectionMaximumAmplification(
    XML_Parser parser, float maximumAmplificationFactor) {
  if ((parser == NULL) || (parser->m_parentParser != NULL)
      || isnan(maximumAmplificationFactor)
      || (maximumAmplificationFactor < 1.0f)) {
    return XML_FALSE;
  }
  parser->m_accounting.maximumAmplificationFactor = maximumAmplificationFactor;
  return XML_TRUE;
}