void *Type_DateTime_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsDateTimeNumber timestamp;
    struct tm * NewDateTime;

    *nItems = 0;
    NewDateTime = (struct tm*) _cmsMalloc(self ->ContextID, sizeof(struct tm));
    if (NewDateTime == NULL) return NULL;

    if (io->Read(io, &timestamp, sizeof(cmsDateTimeNumber), 1) != 1) return NULL;

     _cmsDecodeDateTimeNumber(&timestamp, NewDateTime);

     *nItems = 1;
     return NewDateTime;

     cmsUNUSED_PARAMETER(SizeOfTag);
}