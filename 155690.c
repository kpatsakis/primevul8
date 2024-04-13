testingAccountingGetCountBytesIndirect(XML_Parser parser) {
  if (! parser)
    return 0;
  return parser->m_accounting.countBytesIndirect;
}