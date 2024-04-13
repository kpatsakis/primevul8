SProcXIChangeHierarchy(ClientPtr client)
{
    REQUEST(xXIChangeHierarchyReq);
    swaps(&stuff->length);
    return (ProcXIChangeHierarchy(client));
}