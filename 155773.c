XML_GetErrorCode(XML_Parser parser) {
  if (parser == NULL)
    return XML_ERROR_INVALID_ARGUMENT;
  return parser->m_errorCode;
}