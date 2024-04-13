void *Type_ViewingConditions_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsICCViewingConditions* vc = NULL;

    vc = (cmsICCViewingConditions*) _cmsMallocZero(self ->ContextID, sizeof(cmsICCViewingConditions));
    if (vc == NULL) return NULL;

    *nItems = 0;

    if (!_cmsReadXYZNumber(io, &vc ->IlluminantXYZ)) goto Error;
    if (!_cmsReadXYZNumber(io, &vc ->SurroundXYZ)) goto Error;
    if (!_cmsReadUInt32Number(io, &vc ->IlluminantType)) goto Error;

    *nItems = 1;

    return (void*) vc;

Error:
    if (vc != NULL)
        _cmsFree(self ->ContextID, vc);

    return NULL;

    cmsUNUSED_PARAMETER(SizeOfTag);
}