XML_SetCommentHandler(XML_Parser parser, XML_CommentHandler handler) {
  if (parser != NULL)
    parser->m_commentHandler = handler;
}