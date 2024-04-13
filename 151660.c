void *Type_ColorantTable_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsUInt32Number i, Count;
    cmsNAMEDCOLORLIST* List;
    char Name[34];
    cmsUInt16Number PCS[3];


    if (!_cmsReadUInt32Number(io, &Count)) return NULL;

    if (Count > cmsMAXCHANNELS) {
        cmsSignalError(self->ContextID, cmsERROR_RANGE, "Too many colorants '%d'", Count);
        return NULL;
    }

    List = cmsAllocNamedColorList(self ->ContextID, Count, 0, "", "");
    for (i=0; i < Count; i++) {

        if (io ->Read(io, Name, 32, 1) != 1) goto Error;
        Name[32] = 0;

        if (!_cmsReadUInt16Array(io, 3, PCS)) goto Error;

        if (!cmsAppendNamedColor(List, Name, PCS, NULL)) goto Error;

    }

    *nItems = 1;
    return List;

Error:
    *nItems = 0;
    cmsFreeNamedColorList(List);
    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}