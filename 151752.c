cmsBool  Type_DateTime_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    struct tm * DateTime = (struct tm*) Ptr;
    cmsDateTimeNumber timestamp;

    _cmsEncodeDateTimeNumber(&timestamp, DateTime);
    if (!io ->Write(io, sizeof(cmsDateTimeNumber), &timestamp)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}