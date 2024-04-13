xmlClearNodeInfoSeq(xmlParserNodeInfoSeqPtr seq)
{
    if (seq == NULL)
        return;
    if (seq->buffer != NULL)
        xmlFree(seq->buffer);
    xmlInitNodeInfoSeq(seq);
}