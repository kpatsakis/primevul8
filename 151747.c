cmsBool ReadMPECurve(struct _cms_typehandler_struct* self,
                     cmsIOHANDLER* io,
                     void* Cargo,
                     cmsUInt32Number n,
                     cmsUInt32Number SizeOfTag)
{
      cmsToneCurve** GammaTables = ( cmsToneCurve**) Cargo;

      GammaTables[n] = ReadSegmentedCurve(self, io);
      return (GammaTables[n] != NULL);

      cmsUNUSED_PARAMETER(SizeOfTag);
}