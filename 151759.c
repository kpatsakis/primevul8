void *Type_ProfileSequenceId_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsSEQ* OutSeq;
    cmsUInt32Number Count;
    cmsUInt32Number BaseOffset;

    *nItems = 0;

    // Get actual position as a basis for element offsets
    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // Get table count
    if (!_cmsReadUInt32Number(io, &Count)) return NULL;
    SizeOfTag -= sizeof(cmsUInt32Number);

    // Allocate an empty structure
    OutSeq = cmsAllocProfileSequenceDescription(self ->ContextID, Count);
    if (OutSeq == NULL) return NULL;


    // Read the position table
    if (!ReadPositionTable(self, io, Count, BaseOffset, OutSeq, ReadSeqID)) {

        cmsFreeProfileSequenceDescription(OutSeq);
        return NULL;
    }

    // Success
    *nItems = 1;
    return OutSeq;

}