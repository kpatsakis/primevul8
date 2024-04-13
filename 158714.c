static void handle_pdfname(struct pdf_struct *pdf, struct pdf_obj *obj,
			   const char *pdfname, int escapes,
			   enum objstate *state)
{
    struct pdfname_action *act = NULL;
    unsigned j;
    for (j=0;j<sizeof(pdfname_actions)/sizeof(pdfname_actions[0]);j++) {
	if (!strcmp(pdfname, pdfname_actions[j].pdfname)) {
	    act = &pdfname_actions[j];
	    break;
	}
    }
    if (!act)
	return;
    if (escapes) {
	/* if a commonly used PDF name is escaped that is certainly
	   suspicious. */
	cli_dbgmsg("cli_pdf: pdfname %s is escaped\n", pdfname);
	pdfobj_flag(pdf, obj, ESCAPED_COMMON_PDFNAME);
    }
    if (act->from_state == *state ||
	act->from_state == STATE_ANY) {
	*state = act->to_state;

	if (*state == STATE_FILTER &&
	    act->set_objflag !=OBJ_DICT &&
	    (obj->flags & (1 << act->set_objflag))) {
	    pdfobj_flag(pdf, obj, BAD_STREAM_FILTERS);
	}
	obj->flags |= 1 << act->set_objflag;
    } else {
	/* auto-reset states */
	switch (*state) {
	    case STATE_S:
		*state = STATE_NONE;
		break;
	    default:
		break;
	}
    }
}