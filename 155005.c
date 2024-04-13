xmlParserFindNodeInfoIndex(const xmlParserNodeInfoSeqPtr seq,
                           const xmlNodePtr node)
{
    unsigned long upper, lower, middle;
    int found = 0;

    if ((seq == NULL) || (node == NULL))
        return ((unsigned long) -1);

    /* Do a binary search for the key */
    lower = 1;
    upper = seq->length;
    middle = 0;
    while (lower <= upper && !found) {
        middle = lower + (upper - lower) / 2;
        if (node == seq->buffer[middle - 1].node)
            found = 1;
        else if (node < seq->buffer[middle - 1].node)
            upper = middle - 1;
        else
            lower = middle + 1;
    }

    /* Return position */
    if (middle == 0 || seq->buffer[middle - 1].node < node)
        return middle;
    else
        return middle - 1;
}