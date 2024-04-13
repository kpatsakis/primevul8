htmlParserFinishElementParsing(htmlParserCtxtPtr ctxt) {
    /*
     * Capture end position and add node
     */
    if ( ctxt->node != NULL && ctxt->record_info ) {
       ctxt->nodeInfo->end_pos = ctxt->input->consumed +
                                (CUR_PTR - ctxt->input->base);
       ctxt->nodeInfo->end_line = ctxt->input->line;
       ctxt->nodeInfo->node = ctxt->node;
       xmlParserAddNodeInfo(ctxt, ctxt->nodeInfo);
       htmlNodeInfoPop(ctxt);
    }
    if (!IS_CHAR_CH(CUR)) {
       htmlAutoCloseOnEnd(ctxt);
    }
}