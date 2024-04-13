void *Type_vcgt_Read(struct _cms_typehandler_struct* self,
                     cmsIOHANDLER* io,
                     cmsUInt32Number* nItems,
                     cmsUInt32Number SizeOfTag)
{
    cmsUInt32Number TagType, n, i;
    cmsToneCurve** Curves;

    *nItems = 0;

    // Read tag type
    if (!_cmsReadUInt32Number(io, &TagType)) return NULL;

    // Allocate space for the array
    Curves = ( cmsToneCurve**) _cmsCalloc(self ->ContextID, 3, sizeof(cmsToneCurve*));
    if (Curves == NULL) return NULL;

    // There are two possible flavors
    switch (TagType) {

    // Gamma is stored as a table
    case cmsVideoCardGammaTableType:
    {
       cmsUInt16Number nChannels, nElems, nBytes;

       // Check channel count, which should be 3 (we don't support monochrome this time)
       if (!_cmsReadUInt16Number(io, &nChannels)) goto Error;

       if (nChannels != 3) {
           cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported number of channels for VCGT '%d'", nChannels);
           goto Error;
       }

       // Get Table element count and bytes per element
       if (!_cmsReadUInt16Number(io, &nElems)) goto Error;
       if (!_cmsReadUInt16Number(io, &nBytes)) goto Error;

       // Adobe's quirk fixup. Fixing broken profiles...
       if (nElems == 256 && nBytes == 1 && SizeOfTag == 1576)
           nBytes = 2;


       // Populate tone curves
       for (n=0; n < 3; n++) {

           Curves[n] = cmsBuildTabulatedToneCurve16(self ->ContextID, nElems, NULL);
           if (Curves[n] == NULL) goto Error;

           // On depending on byte depth
           switch (nBytes) {

           // One byte, 0..255
           case 1:
               for (i=0; i < nElems; i++) {

                   cmsUInt8Number v;

                      if (!_cmsReadUInt8Number(io, &v)) goto Error;
                      Curves[n] ->Table16[i] = FROM_8_TO_16(v);
               }
               break;

           // One word 0..65535
           case 2:
              if (!_cmsReadUInt16Array(io, nElems, Curves[n]->Table16)) goto Error;
              break;

          // Unsupported
           default:
              cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported bit depth for VCGT '%d'", nBytes * 8);
              goto Error;
           }
       } // For all 3 channels
    }
    break;

   // In this case, gamma is stored as a formula
   case cmsVideoCardGammaFormulaType:
   {
       _cmsVCGTGAMMA Colorant[3];

        // Populate tone curves
       for (n=0; n < 3; n++) {

           double Params[10];

           if (!_cmsRead15Fixed16Number(io, &Colorant[n].Gamma)) goto Error;
           if (!_cmsRead15Fixed16Number(io, &Colorant[n].Min)) goto Error;
           if (!_cmsRead15Fixed16Number(io, &Colorant[n].Max)) goto Error;

            // Parametric curve type 5 is:
            // Y = (aX + b)^Gamma + e | X >= d
            // Y = cX + f             | X < d

            // vcgt formula is:
            // Y = (Max  Min) * (X ^ Gamma) + Min

            // So, the translation is
            // a = (Max  Min) ^ ( 1 / Gamma)
            // e = Min
            // b=c=d=f=0

           Params[0] = Colorant[n].Gamma;
           Params[1] = pow((Colorant[n].Max - Colorant[n].Min), (1.0 / Colorant[n].Gamma));
           Params[2] = 0;
           Params[3] = 0;
           Params[4] = 0;
           Params[5] = Colorant[n].Min;
           Params[6] = 0;

           Curves[n] = cmsBuildParametricToneCurve(self ->ContextID, 5, Params);
           if (Curves[n] == NULL) goto Error;
       }
   }
   break;

   // Unsupported
   default:
      cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported tag type for VCGT '%d'", TagType);
      goto Error;
   }

   *nItems = 1;
   return (void*) Curves;

// Regret,  free all resources
Error:

    cmsFreeToneCurveTriple(Curves);
    _cmsFree(self ->ContextID, Curves);
    return NULL;

     cmsUNUSED_PARAMETER(SizeOfTag);
}