xmlParserAddNodeInfo(xmlParserCtxtPtr ctxt,
                     const xmlParserNodeInfoPtr info)
{
    unsigned long pos;

    if ((ctxt == NULL) || (info == NULL)) return;

    /* Find pos and check to see if node is already in the sequence */
    pos = xmlParserFindNodeInfoIndex(&ctxt->node_seq, (xmlNodePtr)
                                     info->node);

    if ((pos < ctxt->node_seq.length) &&
        (ctxt->node_seq.buffer != NULL) &&
        (ctxt->node_seq.buffer[pos].node == info->node)) {
        ctxt->node_seq.buffer[pos] = *info;
    }

    /* Otherwise, we need to add new node to buffer */
    else {
        if ((ctxt->node_seq.length + 1 > ctxt->node_seq.maximum) ||
	    (ctxt->node_seq.buffer == NULL)) {
            xmlParserNodeInfo *tmp_buffer;
            unsigned int byte_size;

            if (ctxt->node_seq.maximum == 0)
                ctxt->node_seq.maximum = 2;
            byte_size = (sizeof(*ctxt->node_seq.buffer) *
			(2 * ctxt->node_seq.maximum));

            if (ctxt->node_seq.buffer == NULL)
                tmp_buffer = (xmlParserNodeInfo *) xmlMalloc(byte_size);
            else
                tmp_buffer =
                    (xmlParserNodeInfo *) xmlRealloc(ctxt->node_seq.buffer,
                                                     byte_size);

            if (tmp_buffer == NULL) {
		xmlErrMemory(ctxt, "failed to allocate buffer\n");
                return;
            }
            ctxt->node_seq.buffer = tmp_buffer;
            ctxt->node_seq.maximum *= 2;
        }

        /* If position is not at end, move elements out of the way */
        if (pos != ctxt->node_seq.length) {
            unsigned long i;

            for (i = ctxt->node_seq.length; i > pos; i--)
                ctxt->node_seq.buffer[i] = ctxt->node_seq.buffer[i - 1];
        }

        /* Copy element and increase length */
        ctxt->node_seq.buffer[pos] = *info;
        ctxt->node_seq.length++;
    }
}