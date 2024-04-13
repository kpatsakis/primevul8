int TransformImage(char *cDefInpProf, char *cOutProf)
{
       cmsHPROFILE hIn, hOut, hProof;
       cmsHTRANSFORM xform;
       cmsUInt32Number wInput, wOutput;
       int OutputColorSpace;
       cmsUInt32Number dwFlags = 0;
       cmsUInt32Number EmbedLen;
       cmsUInt8Number* EmbedBuffer;


       cmsSetAdaptationState(ObserverAdaptationState);

       if (BlackPointCompensation) {

            dwFlags |= cmsFLAGS_BLACKPOINTCOMPENSATION;
       }


       switch (PrecalcMode) {

       case 0: dwFlags |= cmsFLAGS_NOOPTIMIZE; break;
       case 2: dwFlags |= cmsFLAGS_HIGHRESPRECALC; break;
       case 3: dwFlags |= cmsFLAGS_LOWRESPRECALC; break;
       default:;
       }


       if (GamutCheck) {
            dwFlags |= cmsFLAGS_GAMUTCHECK;
            cmsSetAlarmCodes(Alarm);
       }

       // Take input color space
       wInput = GetInputPixelType();

        if (lIsDeviceLink) {

            hIn = cmsOpenProfileFromFile(cDefInpProf, "r");
            hOut = NULL;
            hProof = NULL;
       }
        else {

        if (!IgnoreEmbedded && read_icc_profile(&Decompressor, &EmbedBuffer, &EmbedLen))
        {
              hIn = cmsOpenProfileFromMem(EmbedBuffer, EmbedLen);

               if (Verbose) {

                  fprintf(stdout, " (Embedded profile found)\n");
				  PrintProfileInformation(hIn);
                  fflush(stdout);
              }

               if (hIn != NULL && SaveEmbedded != NULL)
                          SaveMemoryBlock(EmbedBuffer, EmbedLen, SaveEmbedded);

              free(EmbedBuffer);
        }
        else
        {
            // Default for ITU/Fax
            if (cDefInpProf == NULL && T_COLORSPACE(wInput) == PT_Lab)
                cDefInpProf = "*Lab";

            if (cDefInpProf != NULL && cmsstrcasecmp(cDefInpProf, "*lab") == 0)
                hIn = CreateITU2PCS_ICC();
            else
                hIn = OpenStockProfile(0, cDefInpProf);
       }

        if (cOutProf != NULL && cmsstrcasecmp(cOutProf, "*lab") == 0)
            hOut = CreatePCS2ITU_ICC();
        else
        hOut = OpenStockProfile(0, cOutProf);

       hProof = NULL;
       if (cProofing != NULL) {

           hProof = OpenStockProfile(0, cProofing);
           if (hProof == NULL) {
            FatalError("Proofing profile couldn't be read.");
           }
           dwFlags |= cmsFLAGS_SOFTPROOFING;
          }
       }

        if (!hIn)
            FatalError("Input profile couldn't be read.");
        if (!hOut)
            FatalError("Output profile couldn't be read.");

       // Assure both, input profile and input JPEG are on same colorspace
       if (cmsGetColorSpace(hIn) != _cmsICCcolorSpace(T_COLORSPACE(wInput)))
              FatalError("Input profile is not operating in proper color space");


       // Output colorspace is given by output profile

        if (lIsDeviceLink) {
            OutputColorSpace = GetDevicelinkColorSpace(hIn);
        }
        else {
            OutputColorSpace = GetProfileColorSpace(hOut);
        }

       jpeg_copy_critical_parameters(&Decompressor, &Compressor);

       WriteOutputFields(OutputColorSpace);

       wOutput      = ComputeOutputFormatDescriptor(wInput, OutputColorSpace);


       xform = cmsCreateProofingTransform(hIn, wInput,
                                          hOut, wOutput,
                                          hProof, Intent,
                                          ProofingIntent, dwFlags);
	   if (xform == NULL)
                 FatalError("Cannot transform by using the profiles");

       DoTransform(xform, OutputColorSpace);


       jcopy_markers_execute(&Decompressor, &Compressor);

       cmsDeleteTransform(xform);
       cmsCloseProfile(hIn);
       cmsCloseProfile(hOut);
       if (hProof) cmsCloseProfile(hProof);

       return 1;
}