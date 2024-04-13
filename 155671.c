moveToFreeBindingList(XML_Parser parser, BINDING *bindings) {
  while (bindings) {
    BINDING *b = bindings;
    bindings = bindings->nextTagBinding;
    b->nextTagBinding = parser->m_freeBindingList;
    parser->m_freeBindingList = b;
  }
}