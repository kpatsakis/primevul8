XML_SetBillionLaughsAttackProtectionActivationThreshold(
    XML_Parser parser, unsigned long long activationThresholdBytes) {
  if ((parser == NULL) || (parser->m_parentParser != NULL)) {
    return XML_FALSE;
  }
  parser->m_accounting.activationThresholdBytes = activationThresholdBytes;
  return XML_TRUE;
}