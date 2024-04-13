cmsBool ReadSeqID(struct _cms_typehandler_struct* self,
                                             cmsIOHANDLER* io,
                                             void* Cargo,
                                             cmsUInt32Number n,
                                             cmsUInt32Number SizeOfTag)
{
    cmsSEQ* OutSeq = (cmsSEQ*) Cargo;
    cmsPSEQDESC* seq = &OutSeq ->seq[n];

    if (io -> Read(io, seq ->ProfileID.ID8, 16, 1) != 1) return FALSE;
    if (!ReadEmbeddedText(self, io, &seq ->Description, SizeOfTag)) return FALSE;

    return TRUE;
}