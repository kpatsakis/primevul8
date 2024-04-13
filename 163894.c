void HandleSwitches(int argc, char *argv[])
{
    int s;

    while ((s=xgetopt(argc,argv,"bBnNvVGgh:H:i:I:o:O:P:p:t:T:c:C:Q:q:M:m:L:l:eEs:S:!:D:d:")) != EOF) {

        switch (s)
        {

        case 'b':
        case 'B':
            BlackPointCompensation = TRUE;
            break;

        case 'd':
        case 'D': ObserverAdaptationState = atof(xoptarg);
            if (ObserverAdaptationState < 0 ||
                ObserverAdaptationState > 1.0)
                FatalError("Adaptation state should be 0..1");
            break;

        case 'v':
        case 'V':
            Verbose = TRUE;
            break;

        case 'i':
        case 'I':
            if (lIsDeviceLink)
                FatalError("Device-link already specified");

            cInpProf = xoptarg;
            break;

        case 'o':
        case 'O':
            if (lIsDeviceLink)
                FatalError("Device-link already specified");

            cOutProf = xoptarg;
            break;

        case 'l':
        case 'L':
			if (cInpProf != NULL || cOutProf != NULL)
				FatalError("input/output profiles already specified");

            cInpProf = xoptarg;
            lIsDeviceLink = TRUE;
            break;

        case 'p':
        case 'P':
            cProofing = xoptarg;
            break;

        case 't':
        case 'T':
            Intent = atoi(xoptarg);
            break;

        case 'N':
        case 'n':
            IgnoreEmbedded = TRUE;
            break;

        case 'e':
        case 'E':
            EmbedProfile = TRUE;
            break;


        case 'g':
        case 'G':
            GamutCheck = TRUE;
            break;

        case 'c':
        case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 2)
                FatalError("Unknown precalc mode '%d'", PrecalcMode);
            break;

        case 'H':
        case 'h':  {

            int a =  atoi(xoptarg);
            Help(a);
                   }
            break;

        case 'q':
        case 'Q':
            jpegQuality = atoi(xoptarg);
            if (jpegQuality > 100) jpegQuality = 100;
            if (jpegQuality < 0)   jpegQuality = 0;
            break;

        case 'm':
        case 'M':
            ProofingIntent = atoi(xoptarg);
            break;

        case 's':
        case 'S': SaveEmbedded = xoptarg;
            break;

        case '!':
            if (sscanf(xoptarg, "%hu,%hu,%hu", &Alarm[0], &Alarm[1], &Alarm[2]) == 3) {
                int i;
                for (i=0; i < 3; i++) {
                    Alarm[i] = (Alarm[i] << 8) | Alarm[i];
                }
            }
            break;

        default:

            FatalError("Unknown option - run without args to see valid ones");
        }

    }
}