cmsToneCurve* ReadSegmentedCurve(struct _cms_typehandler_struct* self, cmsIOHANDLER* io)
{
    cmsCurveSegSignature ElementSig;
    cmsUInt32Number i, j;
    cmsUInt16Number nSegments;
    cmsCurveSegment*  Segments;
    cmsToneCurve* Curve;
    cmsFloat32Number PrevBreak = MINUS_INF;    // - infinite

    // Take signature and channels for each element.
     if (!_cmsReadUInt32Number(io, (cmsUInt32Number*) &ElementSig)) return NULL;

     // That should be a segmented curve
     if (ElementSig != cmsSigSegmentedCurve) return NULL;

     if (!_cmsReadUInt32Number(io, NULL)) return NULL;
     if (!_cmsReadUInt16Number(io, &nSegments)) return NULL;
     if (!_cmsReadUInt16Number(io, NULL)) return NULL;

     if (nSegments < 1) return NULL;
     Segments = (cmsCurveSegment*) _cmsCalloc(self ->ContextID, nSegments, sizeof(cmsCurveSegment));
     if (Segments == NULL) return NULL;

     // Read breakpoints
     for (i=0; i < (cmsUInt32Number) nSegments - 1; i++) {

         Segments[i].x0 = PrevBreak;
         if (!_cmsReadFloat32Number(io, &Segments[i].x1)) goto Error;
         PrevBreak = Segments[i].x1;
     }

     Segments[nSegments-1].x0 = PrevBreak;
     Segments[nSegments-1].x1 = PLUS_INF;     // A big cmsFloat32Number number

     // Read segments
     for (i=0; i < nSegments; i++) {

          if (!_cmsReadUInt32Number(io, (cmsUInt32Number*) &ElementSig)) goto Error;
          if (!_cmsReadUInt32Number(io, NULL)) goto Error;

           switch (ElementSig) {

            case cmsSigFormulaCurveSeg: {

                cmsUInt16Number Type;
                cmsUInt32Number ParamsByType[] = {4, 5, 5 };

                if (!_cmsReadUInt16Number(io, &Type)) goto Error;
                if (!_cmsReadUInt16Number(io, NULL)) goto Error;

                Segments[i].Type = Type + 6;
                if (Type > 2) goto Error;

                for (j=0; j < ParamsByType[Type]; j++) {

                    cmsFloat32Number f;
                    if (!_cmsReadFloat32Number(io, &f)) goto Error;
                    Segments[i].Params[j] = f;
                }
                }
                break;


            case cmsSigSampledCurveSeg: {
                cmsUInt32Number Count;

                if (!_cmsReadUInt32Number(io, &Count)) goto Error;

                Segments[i].nGridPoints = Count;
                Segments[i].SampledPoints = (cmsFloat32Number*) _cmsCalloc(self ->ContextID, Count, sizeof(cmsFloat32Number));
                if (Segments[i].SampledPoints == NULL) goto Error;

                for (j=0; j < Count; j++) {
                    if (!_cmsReadFloat32Number(io, &Segments[i].SampledPoints[j])) goto Error;
                }
                }
                break;

            default:
                {
                char String[5];

                _cmsTagSignature2String(String, (cmsTagSignature) ElementSig);
                cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown curve element type '%s' found.", String);
                }
                goto Error;

         }
     }

     Curve = cmsBuildSegmentedToneCurve(self ->ContextID, nSegments, Segments);

     for (i=0; i < nSegments; i++) {
         if (Segments[i].SampledPoints) _cmsFree(self ->ContextID, Segments[i].SampledPoints);
     }
     _cmsFree(self ->ContextID, Segments);
     return Curve;

Error:
     if (Segments) {
         for (i=0; i < nSegments; i++) {
             if (Segments[i].SampledPoints) _cmsFree(self ->ContextID, Segments[i].SampledPoints);
         }
         _cmsFree(self ->ContextID, Segments);
     }
     return NULL;
}