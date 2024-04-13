cmsBool ReadOneMLUC(struct _cms_typehandler_struct* self, cmsIOHANDLER* io,  _cmsDICelem* e, cmsUInt32Number i, cmsMLU** mlu)
{
    cmsUInt32Number nItems = 0;

    // A way to get null MLUCs
    if (e -> Offsets[i] == 0 || e ->Sizes[i] == 0) {

        *mlu = NULL;
        return TRUE;
    }

    if (!io -> Seek(io, e -> Offsets[i])) return FALSE;

    *mlu = (cmsMLU*) Type_MLU_Read(self, io, &nItems, e ->Sizes[i]);
    return *mlu != NULL;
}