cmsBool WriteOneMLUC(struct _cms_typehandler_struct* self, cmsIOHANDLER* io,  _cmsDICelem* e, cmsUInt32Number i, const cmsMLU* mlu, cmsUInt32Number BaseOffset)
{
    cmsUInt32Number Before;

     // Special case for undefined strings (see ICC Votable
     // Proposal Submission, Dictionary Type and Metadata TAG Definition)
     if (mlu == NULL) {
        e ->Sizes[i] = 0;
        e ->Offsets[i] = 0;
        return TRUE;
    }

    Before = io ->Tell(io);
    e ->Offsets[i] = Before - BaseOffset;

    if (!Type_MLU_Write(self, io, (void*) mlu, 1)) return FALSE;

    e ->Sizes[i] = io ->Tell(io) - Before;
    return TRUE;
}