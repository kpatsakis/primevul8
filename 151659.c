void *Type_XYZ_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsCIEXYZ* xyz;

    *nItems = 0;
    xyz = (cmsCIEXYZ*) _cmsMallocZero(self ->ContextID, sizeof(cmsCIEXYZ));
    if (xyz == NULL) return NULL;

    if (!_cmsReadXYZNumber(io, xyz)) {
        _cmsFree(self ->ContextID, xyz);
        return NULL;
    }

    *nItems = 1;
    return (void*) xyz;

    cmsUNUSED_PARAMETER(SizeOfTag);
}