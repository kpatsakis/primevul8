cmsToneCurve* ReadEmbeddedCurve(struct _cms_typehandler_struct* self, cmsIOHANDLER* io)
{
    cmsTagTypeSignature  BaseType;
    cmsUInt32Number nItems;

    BaseType = _cmsReadTypeBase(io);
    switch (BaseType) {

            case cmsSigCurveType:
                return (cmsToneCurve*) Type_Curve_Read(self, io, &nItems, 0);

            case cmsSigParametricCurveType:
                return (cmsToneCurve*) Type_ParametricCurve_Read(self, io, &nItems, 0);

            default:
                {
                    char String[5];

                    _cmsTagSignature2String(String, (cmsTagSignature) BaseType);
                    cmsSignalError(self ->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown curve type '%s'", String);
                }
                return NULL;
    }
}