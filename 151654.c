cmsBool  Type_ProfileSequenceId_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsSEQ* Seq = (cmsSEQ*) Ptr;
    cmsUInt32Number BaseOffset;

    // Keep the base offset
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // This is the table count
    if (!_cmsWriteUInt32Number(io, Seq ->n)) return FALSE;

    // This is the position table and content
    if (!WritePositionTable(self, io, 0, Seq ->n, BaseOffset, Seq, WriteSeqID)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}