accountingGetCurrentAmplification(XML_Parser rootParser) {
  const XmlBigCount countBytesOutput
      = rootParser->m_accounting.countBytesDirect
        + rootParser->m_accounting.countBytesIndirect;
  const float amplificationFactor
      = rootParser->m_accounting.countBytesDirect
            ? (countBytesOutput
               / (float)(rootParser->m_accounting.countBytesDirect))
            : 1.0f;
  assert(! rootParser->m_parentParser);
  return amplificationFactor;
}