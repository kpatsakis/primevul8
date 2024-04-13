xmlClearParserCtxt(xmlParserCtxtPtr ctxt)
{
  if (ctxt==NULL)
    return;
  xmlClearNodeInfoSeq(&ctxt->node_seq);
  xmlCtxtReset(ctxt);
}