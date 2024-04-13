htmlSkipBlankChars(xmlParserCtxtPtr ctxt) {
    int res = 0;

    while (IS_BLANK_CH(*(ctxt->input->cur))) {
	if ((*ctxt->input->cur == 0) &&
	    (xmlParserInputGrow(ctxt->input, INPUT_CHUNK) <= 0)) {
		xmlPopInput(ctxt);
	} else {
	    if (*(ctxt->input->cur) == '\n') {
		ctxt->input->line++; ctxt->input->col = 1;
	    } else ctxt->input->col++;
	    ctxt->input->cur++;
	    ctxt->nbChars++;
	    if (*ctxt->input->cur == 0)
		xmlParserInputGrow(ctxt->input, INPUT_CHUNK);
	}
	res++;
    }
    return(res);
}