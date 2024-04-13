xmlSwitchToEncoding(xmlParserCtxtPtr ctxt, xmlCharEncodingHandlerPtr handler)
{
    return (xmlSwitchToEncodingInt(ctxt, handler, -1));
}