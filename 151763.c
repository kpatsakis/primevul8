cmsBool Type_Dictionary_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsHANDLE hDict = (cmsHANDLE) Ptr;
    const cmsDICTentry* p;
    cmsBool AnyName, AnyValue;
    cmsUInt32Number i, Count, Length;
    cmsUInt32Number DirectoryPos, CurrentPos, BaseOffset;
   _cmsDICarray a;

    if (hDict == NULL) return FALSE;

    BaseOffset = io ->Tell(io) - sizeof(_cmsTagBase);

    // Let's inspect the dictionary
    Count = 0; AnyName = FALSE; AnyValue = FALSE;
    for (p = cmsDictGetEntryList(hDict); p != NULL; p = cmsDictNextEntry(p)) {

        if (p ->DisplayName != NULL) AnyName = TRUE;
        if (p ->DisplayValue != NULL) AnyValue = TRUE;
        Count++;
    }

    Length = 16;
    if (AnyName)  Length += 8;
    if (AnyValue) Length += 8;

    if (!_cmsWriteUInt32Number(io, Count)) return FALSE;
    if (!_cmsWriteUInt32Number(io, Length)) return FALSE;

    // Keep starting position of offsets table
    DirectoryPos = io ->Tell(io);

    // Allocate offsets array
    if (!AllocArray(self ->ContextID, &a, Count, Length)) goto Error;

    // Write a fake directory to be filled latter on
    if (!WriteOffsetArray(io, &a, Count, Length)) goto Error;

    // Write each element. Keep track of the size as well.
    p = cmsDictGetEntryList(hDict);
    for (i=0; i < Count; i++) {

        if (!WriteOneWChar(io, &a.Name, i,  p ->Name, BaseOffset)) goto Error;
        if (!WriteOneWChar(io, &a.Value, i, p ->Value, BaseOffset)) goto Error;

        if (p ->DisplayName != NULL) {
            if (!WriteOneMLUC(self, io, &a.DisplayName, i, p ->DisplayName, BaseOffset)) goto Error;
        }

        if (p ->DisplayValue != NULL) {
            if (!WriteOneMLUC(self, io, &a.DisplayValue, i, p ->DisplayValue, BaseOffset)) goto Error;
        }

       p = cmsDictNextEntry(p);
    }

    // Write the directory
    CurrentPos = io ->Tell(io);
    if (!io ->Seek(io, DirectoryPos)) goto Error;

    if (!WriteOffsetArray(io, &a, Count, Length)) goto Error;

    if (!io ->Seek(io, CurrentPos)) goto Error;

    FreeArray(&a);
    return TRUE;

Error:
    FreeArray(&a);
    return FALSE;

    cmsUNUSED_PARAMETER(nItems);
}