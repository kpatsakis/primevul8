SProcXChangeFeedbackControl(ClientPtr client)
{
    REQUEST(xChangeFeedbackControlReq);
    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xChangeFeedbackControlReq);
    swapl(&stuff->mask);
    return (ProcXChangeFeedbackControl(client));
}