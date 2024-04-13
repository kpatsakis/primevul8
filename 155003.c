xmlSwitchInputEncoding(xmlParserCtxtPtr ctxt, xmlParserInputPtr input,
                          xmlCharEncodingHandlerPtr handler) {
    return(xmlSwitchInputEncodingInt(ctxt, input, handler, -1));
}