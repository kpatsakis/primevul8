void *Type_U16Fixed16_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    cmsFloat64Number*  array_double;
    cmsUInt32Number v;
    cmsUInt32Number i, n;

    *nItems = 0;
    n = SizeOfTag / sizeof(cmsUInt32Number);
    array_double = (cmsFloat64Number*) _cmsCalloc(self ->ContextID, n, sizeof(cmsFloat64Number));
    if (array_double == NULL) return NULL;

    for (i=0; i < n; i++) {

        if (!_cmsReadUInt32Number(io, &v)) {
            _cmsFree(self ->ContextID, (void*) array_double);
            return NULL;
        }

        // Convert to cmsFloat64Number
        array_double[i] =  (cmsFloat64Number) (v / 65536.0);
    }

    *nItems = n;
    return (void*) array_double;
}