cmsBool Type_ViewingConditions_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsICCViewingConditions* sc = (cmsICCViewingConditions* ) Ptr;

    if (!_cmsWriteXYZNumber(io, &sc ->IlluminantXYZ)) return FALSE;
    if (!_cmsWriteXYZNumber(io, &sc ->SurroundXYZ)) return FALSE;
    if (!_cmsWriteUInt32Number(io, sc ->IlluminantType)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}