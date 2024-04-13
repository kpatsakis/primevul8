void *Type_Data_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsICCData* BinData;
    cmsUInt32Number LenOfData;

    *nItems = 0;

    if (SizeOfTag < sizeof(cmsUInt32Number)) return NULL;

    LenOfData = SizeOfTag - sizeof(cmsUInt32Number);
    if (LenOfData > INT_MAX) return NULL;

    BinData = (cmsICCData*) _cmsMalloc(self ->ContextID, sizeof(cmsICCData) + LenOfData - 1);
    if (BinData == NULL) return NULL;

    BinData ->len = LenOfData;
    if (!_cmsReadUInt32Number(io, &BinData->flag)) {
        _cmsFree(self ->ContextID, BinData);
        return NULL;
    }

    if (io -> Read(io, BinData ->data, sizeof(cmsUInt8Number), LenOfData) != LenOfData) {

        _cmsFree(self ->ContextID, BinData);
        return NULL;
    }

    *nItems = 1;

    return (void*) BinData;
}