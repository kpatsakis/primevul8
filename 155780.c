destroyBindings(BINDING *bindings, XML_Parser parser) {
  for (;;) {
    BINDING *b = bindings;
    if (! b)
      break;
    bindings = b->nextTagBinding;
    FREE(parser, b->uri);
    FREE(parser, b);
  }
}