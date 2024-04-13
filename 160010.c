void print_verbose(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	id2hr_t *MountName, *FormatName, *Aspect, *Crop, *DriveMode;
	int ret;

	if ((ret = MyCoolRawProcessor.adjust_sizes_info_only()))
	{
		fprintf(outfile, "Cannot decode %s: %s\n", fn.c_str(), libraw_strerror(ret));
		return; // no recycle, open_file will recycle
	}

	fprintf(outfile, "\nFilename: %s\n", fn.c_str());
	if (C.OriginalRawFileName[0])
		fprintf(outfile, "OriginalRawFileName: =%s=\n", C.OriginalRawFileName);
	fprintf(outfile, "Timestamp: %s", ctime(&(P2.timestamp)));
	fprintf(outfile, "Camera: %s %s ID: 0x%llx\n", P1.make, P1.model, mnLens.CamID);
	fprintf(outfile, "Normalized Make/Model: =%s/%s= ", P1.normalized_make,
		P1.normalized_model);
	fprintf(outfile, "CamMaker ID: %d, ", P1.maker_index);
	const char *CamMakerName = LibRaw::cameramakeridx2maker(P1.maker_index);
	if (CamMakerName)
		fprintf(outfile, "%s\n", CamMakerName);
	else
		fprintf(outfile, "Undefined\n");
	{
		int i = 0;
		char sep[] = ", ";
		if (C.UniqueCameraModel[0]) {
			i++;
			fprintf(outfile, "UniqueCameraModel: =%s=", C.UniqueCameraModel);
		}
		if (C.LocalizedCameraModel[0]) {
			if (i) {
				fprintf(outfile, "%s", sep);
				i++;
			}
			fprintf(outfile, "LocalizedCameraModel: =%s=", C.LocalizedCameraModel);
		}
		if (i) {
			fprintf(outfile, "\n");
			i = 0;
		}
		if (C.ImageUniqueID[0]) {
			if (i) fprintf(outfile, "%s", sep);
			i++;
			fprintf(outfile, "ImageUniqueID: =%s=", C.ImageUniqueID);
		}
		if (C.RawDataUniqueID[0]) {
			if (i) fprintf(outfile, "%s", sep);
			i++;
			fprintf(outfile, "RawDataUniqueID: =%s=", C.RawDataUniqueID);
		}
		if (i) fprintf(outfile, "\n");
	}

	if (ShootingInfo.BodySerial[0] && strcmp(ShootingInfo.BodySerial, "0"))
	{
		trimSpaces(ShootingInfo.BodySerial);
		fprintf(outfile, "Body#: %s", ShootingInfo.BodySerial);
	}
	else if (C.model2[0] && (!strncasecmp(P1.normalized_make, "Kodak", 5)))
	{
		trimSpaces(C.model2);
		fprintf(outfile, "Body#: %s", C.model2);
	}
	if (ShootingInfo.InternalBodySerial[0])
	{
		trimSpaces(ShootingInfo.InternalBodySerial);
		fprintf(outfile, " BodyAssy#: %s", ShootingInfo.InternalBodySerial);
	}
	if (exifLens.LensSerial[0])
	{
		trimSpaces(exifLens.LensSerial);
		fprintf(outfile, " Lens#: %s", exifLens.LensSerial);
	}
	if (exifLens.InternalLensSerial[0])
	{
		trimSpaces(exifLens.InternalLensSerial);
		fprintf(outfile, " LensAssy#: %s", exifLens.InternalLensSerial);
	}
	if (P2.artist[0])
		fprintf(outfile, " Owner: %s\n", P2.artist);
	if (P1.dng_version)
	{
		fprintf(outfile, " DNG Version: ");
		for (int i = 24; i >= 0; i -= 8)
			fprintf(outfile, "%d%c", P1.dng_version >> i & 255, i ? '.' : '\n');
	}
	fprintf(outfile, "\nEXIF:\n");
	fprintf(outfile, "\tMinFocal: %0.1f mm\n", exifLens.MinFocal);
	fprintf(outfile, "\tMaxFocal: %0.1f mm\n", exifLens.MaxFocal);
	fprintf(outfile, "\tMaxAp @MinFocal: f/%0.1f\n", exifLens.MaxAp4MinFocal);
	fprintf(outfile, "\tMaxAp @MaxFocal: f/%0.1f\n", exifLens.MaxAp4MaxFocal);
	fprintf(outfile, "\tMaxAperture @CurFocal: f/%0.1f\n", exifLens.EXIF_MaxAp);
	fprintf(outfile, "\tFocalLengthIn35mmFormat: %d mm\n",
		exifLens.FocalLengthIn35mmFormat);
	fprintf(outfile, "\tLensMake: %s\n", exifLens.LensMake);
	fprintf(outfile, "\tLens: %s\n", exifLens.Lens);
	fprintf(outfile, "\n");

	fprintf(outfile, "\nMakernotes:\n");
	fprintf(outfile, "\tDriveMode: %d\n", ShootingInfo.DriveMode);
	fprintf(outfile, "\tFocusMode: %d\n", ShootingInfo.FocusMode);
	fprintf(outfile, "\tMeteringMode: %d\n", ShootingInfo.MeteringMode);
	fprintf(outfile, "\tAFPoint: %d\n", ShootingInfo.AFPoint);
	fprintf(outfile, "\tExposureMode: %d\n", ShootingInfo.ExposureMode);
	fprintf(outfile, "\tExposureProgram: %d\n", ShootingInfo.ExposureProgram);
	fprintf(outfile, "\tImageStabilization: %d\n", ShootingInfo.ImageStabilization);
	if (mnLens.body[0])
	{
		fprintf(outfile, "\tHost Body: %s\n", mnLens.body);
	}
	if (Hasselblad.CaptureSequenceInitiator[0])
	{
		fprintf(outfile, "\tInitiator: %s\n", Hasselblad.CaptureSequenceInitiator);
	}
	if (Hasselblad.SensorUnitConnector[0])
	{
		fprintf(outfile, "\tSU Connector: %s\n", Hasselblad.SensorUnitConnector);
	}
	fprintf(outfile, "\tCameraFormat: %d, ", mnLens.CameraFormat);
	FormatName = lookup_id2hr(mnLens.CameraFormat, FormatNames, nFormats);
	if (FormatName)
		fprintf(outfile, "%s\n", FormatName->name);
	else
		fprintf(outfile, "Unknown\n");

	if (!strncasecmp(P1.make, "Nikon", 5) && Nikon.SensorHighSpeedCrop.cwidth)
	{
		fprintf(outfile, "\tNikon crop: %d, ", Nikon.HighSpeedCropFormat);
		Crop = lookup_id2hr(Nikon.HighSpeedCropFormat, NikonCrops, nNikonCrops);
		if (Crop)
			fprintf(outfile, "%s\n", Crop->name);
		else
			fprintf(outfile, "Unknown\n");
		fprintf(outfile, "\tSensor used area %d x %d; crop from: %d x %d at top left "
			"pixel: (%d, %d)\n",
			Nikon.SensorWidth, Nikon.SensorHeight,
			Nikon.SensorHighSpeedCrop.cwidth,
			Nikon.SensorHighSpeedCrop.cheight,
			Nikon.SensorHighSpeedCrop.cleft, Nikon.SensorHighSpeedCrop.ctop);
	}

	fprintf(outfile, "\tCameraMount: %d, ", mnLens.CameraMount);
	MountName = lookup_id2hr(mnLens.CameraMount, MountNames, nMounts);
	if (MountName)
		fprintf(outfile, "%s\n", MountName->name);
	else
		fprintf(outfile, "Unknown\n");

	if (mnLens.LensID == -1)
		fprintf(outfile, "\tLensID: n/a\n");
	else
		fprintf(outfile, "\tLensID: %llu 0x%0llx\n", mnLens.LensID, mnLens.LensID);

	fprintf(outfile, "\tLens: %s\n", mnLens.Lens);
	fprintf(outfile, "\tLensFormat: %d, ", mnLens.LensFormat);
	FormatName = lookup_id2hr(mnLens.LensFormat, FormatNames, nFormats);
	if (FormatName)
		fprintf(outfile, "%s\n", FormatName->name);
	else
		fprintf(outfile, "Unknown\n");

	fprintf(outfile, "\tLensMount: %d, ", mnLens.LensMount);
	MountName = lookup_id2hr(mnLens.LensMount, MountNames, nMounts);
	if (MountName)
		fprintf(outfile, "%s\n", MountName->name);
	else
		fprintf(outfile, "Unknown\n");

	fprintf(outfile, "\tFocalType: %d, ", mnLens.FocalType);
	switch (mnLens.FocalType)
	{
	case LIBRAW_FT_UNDEFINED:
		fprintf(outfile, "Undefined\n");
		break;
	case LIBRAW_FT_PRIME_LENS:
		fprintf(outfile, "Prime lens\n");
		break;
	case LIBRAW_FT_ZOOM_LENS:
		fprintf(outfile, "Zoom lens\n");
		break;
	default:
		fprintf(outfile, "Unknown\n");
		break;
	}
	fprintf(outfile, "\tLensFeatures_pre: %s\n", mnLens.LensFeatures_pre);
	fprintf(outfile, "\tLensFeatures_suf: %s\n", mnLens.LensFeatures_suf);
	fprintf(outfile, "\tMinFocal: %0.1f mm\n", mnLens.MinFocal);
	fprintf(outfile, "\tMaxFocal: %0.1f mm\n", mnLens.MaxFocal);
	fprintf(outfile, "\tMaxAp @MinFocal: f/%0.1f\n", mnLens.MaxAp4MinFocal);
	fprintf(outfile, "\tMaxAp @MaxFocal: f/%0.1f\n", mnLens.MaxAp4MaxFocal);
	fprintf(outfile, "\tMinAp @MinFocal: f/%0.1f\n", mnLens.MinAp4MinFocal);
	fprintf(outfile, "\tMinAp @MaxFocal: f/%0.1f\n", mnLens.MinAp4MaxFocal);
	fprintf(outfile, "\tMaxAp: f/%0.1f\n", mnLens.MaxAp);
	fprintf(outfile, "\tMinAp: f/%0.1f\n", mnLens.MinAp);
	fprintf(outfile, "\tCurFocal: %0.1f mm\n", mnLens.CurFocal);
	fprintf(outfile, "\tCurAp: f/%0.1f\n", mnLens.CurAp);
	fprintf(outfile, "\tMaxAp @CurFocal: f/%0.1f\n", mnLens.MaxAp4CurFocal);
	fprintf(outfile, "\tMinAp @CurFocal: f/%0.1f\n", mnLens.MinAp4CurFocal);

	if (exifLens.makernotes.FocalLengthIn35mmFormat > 1.0f)
		fprintf(outfile, "\tFocalLengthIn35mmFormat: %0.1f mm\n",
		exifLens.makernotes.FocalLengthIn35mmFormat);

	if (exifLens.nikon.EffectiveMaxAp > 0.1f)
		fprintf(outfile, "\tEffectiveMaxAp: f/%0.1f\n",
		exifLens.nikon.EffectiveMaxAp);

	if (exifLens.makernotes.LensFStops > 0.1f)
		fprintf(outfile, "\tLensFStops @CurFocal: %0.2f\n",
		exifLens.makernotes.LensFStops);

	fprintf(outfile, "\tTeleconverterID: %lld\n", mnLens.TeleconverterID);
	fprintf(outfile, "\tTeleconverter: %s\n", mnLens.Teleconverter);
	fprintf(outfile, "\tAdapterID: %lld\n", mnLens.AdapterID);
	fprintf(outfile, "\tAdapter: %s\n", mnLens.Adapter);
	fprintf(outfile, "\tAttachmentID: %lld\n", mnLens.AttachmentID);
	fprintf(outfile, "\tAttachment: %s\n", mnLens.Attachment);
	fprintf(outfile, "\n");

	fprintf(outfile, "ISO speed: %d\n", (int)P2.iso_speed);
	if (P3.real_ISO > 0.1f)
		fprintf(outfile, "real ISO speed: %d\n", (int)P3.real_ISO);
	fprintf(outfile, "Shutter: ");
	if (P2.shutter > 0 && P2.shutter < 1)
		P2.shutter = fprintf(outfile, "1/%0.1f\n", 1.0f / P2.shutter);
	else if (P2.shutter >= 1)
		fprintf(outfile, "%0.1f sec\n", P2.shutter);
	else /* negative*/
		fprintf(outfile, " negative value\n");
	fprintf(outfile, "Aperture: f/%0.1f\n", P2.aperture);
	fprintf(outfile, "Focal length: %0.1f mm\n", P2.focal_len);
	if (P3.exifAmbientTemperature > -273.15f)
		fprintf(outfile, "Ambient temperature (exif data): %6.2f° C\n",
		P3.exifAmbientTemperature);
	if (P3.CameraTemperature > -273.15f)
		fprintf(outfile, "Camera temperature: %6.2f° C\n", P3.CameraTemperature);
	if (P3.SensorTemperature > -273.15f)
		fprintf(outfile, "Sensor temperature: %6.2f° C\n", P3.SensorTemperature);
	if (P3.SensorTemperature2 > -273.15f)
		fprintf(outfile, "Sensor temperature2: %6.2f° C\n", P3.SensorTemperature2);
	if (P3.LensTemperature > -273.15f)
		fprintf(outfile, "Lens temperature: %6.2f° C\n", P3.LensTemperature);
	if (P3.AmbientTemperature > -273.15f)
		fprintf(outfile, "Ambient temperature: %6.2f° C\n", P3.AmbientTemperature);
	if (P3.BatteryTemperature > -273.15f)
		fprintf(outfile, "Battery temperature: %6.2f° C\n", P3.BatteryTemperature);
	if (P3.FlashGN > 1.0f)
		fprintf(outfile, "Flash Guide Number: %6.2f\n", P3.FlashGN);
	fprintf(outfile, "Flash exposure compensation: %0.2f EV\n", P3.FlashEC);
	if (C.profile)
		fprintf(outfile, "Embedded ICC profile: yes, %d bytes\n", C.profile_length);
	else
		fprintf(outfile, "Embedded ICC profile: no\n");

	if (C.dng_levels.baseline_exposure > -999.f)
		fprintf(outfile, "Baseline exposure: %04.3f\n", C.dng_levels.baseline_exposure);

	fprintf(outfile, "Number of raw images: %d\n", P1.raw_count);

	if (Fuji.DriveMode != -1)
	{
		fprintf(outfile, "Fuji Drive Mode: %d, ", Fuji.DriveMode);
		DriveMode =
			lookup_id2hr(Fuji.DriveMode, FujiDriveModes, nFujiDriveModes);
		if (DriveMode)
			fprintf(outfile, "%s\n", DriveMode->name);
		else
			fprintf(outfile, "Unknown\n");
	}

	if (Fuji.CropMode)
	{
		fprintf(outfile, "Fuji Crop Mode: %d, ", Fuji.CropMode);
		Crop = lookup_id2hr(Fuji.CropMode, FujiCrops, nFujiCrops);
		if (Crop)
			fprintf(outfile, "%s\n", Crop->name);
		else
			fprintf(outfile, "Unknown\n");
	}

	if (Fuji.ExpoMidPointShift > -999.f)
		fprintf(outfile, "Fuji Exposure shift: %04.3f\n", Fuji.ExpoMidPointShift);

	if (Fuji.DynamicRange != 0xffff)
		fprintf(outfile, "Fuji Dynamic Range: %d\n", Fuji.DynamicRange);
	if (Fuji.FilmMode != 0xffff)
		fprintf(outfile, "Fuji Film Mode: %d\n", Fuji.FilmMode);
	if (Fuji.DynamicRangeSetting != 0xffff)
		fprintf(outfile, "Fuji Dynamic Range Setting: %d\n",
		Fuji.DynamicRangeSetting);
	if (Fuji.DevelopmentDynamicRange != 0xffff)
		fprintf(outfile, "Fuji Development Dynamic Range: %d\n",
		Fuji.DevelopmentDynamicRange);
	if (Fuji.AutoDynamicRange != 0xffff)
		fprintf(outfile, "Fuji Auto Dynamic Range: %d\n", Fuji.AutoDynamicRange);

	if (S.pixel_aspect != 1)
		fprintf(outfile, "Pixel Aspect Ratio: %0.6f\n", S.pixel_aspect);
	if (T.tlength)
		fprintf(outfile, "Thumb size:  %4d x %d\n", T.twidth, T.theight);
	fprintf(outfile, "Full size:   %4d x %d\n", S.raw_width, S.raw_height);

	if (S.raw_inset_crop.cwidth)
	{
		fprintf(outfile, "Raw inset, width x height: %4d x %d ", S.raw_inset_crop.cwidth,
			S.raw_inset_crop.cheight);
		if (S.raw_inset_crop.cleft != 0xffff)
			fprintf(outfile, "left: %d ", S.raw_inset_crop.cleft);
		if (S.raw_inset_crop.ctop != 0xffff)
			fprintf(outfile, "top: %d", S.raw_inset_crop.ctop);
		fprintf(outfile, "\n");
	}

	fprintf(outfile, "Aspect: ");
	Aspect =
		lookup_id2hr(S.raw_inset_crop.aspect, AspectRatios, nAspectRatios);
	if (Aspect)
		fprintf(outfile, "%s\n", Aspect->name);
	else
		fprintf(outfile, "Other %d\n", S.raw_inset_crop.aspect);

	fprintf(outfile, "Image size:  %4d x %d\n", S.width, S.height);
	fprintf(outfile, "Output size: %4d x %d\n", S.iwidth, S.iheight);
	fprintf(outfile, "Image flip: %d\n", S.flip);

	if (Canon.SensorWidth)
		fprintf(outfile, "SensorWidth          = %d\n", Canon.SensorWidth);
	if (Canon.SensorHeight)
		fprintf(outfile, "SensorHeight         = %d\n", Canon.SensorHeight);
	if (Canon.SensorLeftBorder != -1)
		fprintf(outfile, "SensorLeftBorder     = %d\n", Canon.SensorLeftBorder);
	if (Canon.SensorTopBorder != -1)
		fprintf(outfile, "SensorTopBorder      = %d\n", Canon.SensorTopBorder);
	if (Canon.SensorRightBorder)
		fprintf(outfile, "SensorRightBorder    = %d\n", Canon.SensorRightBorder);
	if (Canon.SensorBottomBorder)
		fprintf(outfile, "SensorBottomBorder   = %d\n", Canon.SensorBottomBorder);
	if (Canon.BlackMaskLeftBorder)
		fprintf(outfile, "BlackMaskLeftBorder  = %d\n", Canon.BlackMaskLeftBorder);
	if (Canon.BlackMaskTopBorder)
		fprintf(outfile, "BlackMaskTopBorder   = %d\n", Canon.BlackMaskTopBorder);
	if (Canon.BlackMaskRightBorder)
		fprintf(outfile, "BlackMaskRightBorder = %d\n", Canon.BlackMaskRightBorder);
	if (Canon.BlackMaskBottomBorder)
		fprintf(outfile, "BlackMaskBottomBorder= %d\n", Canon.BlackMaskBottomBorder);

	if (Hasselblad.BaseISO)
		fprintf(outfile, "Hasselblad base ISO: %d\n", Hasselblad.BaseISO);
	if (Hasselblad.Gain)
		fprintf(outfile, "Hasselblad gain: %g\n", Hasselblad.Gain);

	fprintf(outfile, "Raw colors: %d", P1.colors);
	if (P1.filters)
	{
		fprintf(outfile, "\nFilter pattern: ");
		if (!P1.cdesc[3])
			P1.cdesc[3] = 'G';
		for (int i = 0; i < 16; i++)
			putchar(P1.cdesc[MyCoolRawProcessor.fcol(i >> 1, i & 1)]);
	}

	if (Canon.ChannelBlackLevel[0])
		fprintf(outfile, "\nCanon makernotes, ChannelBlackLevel: %d %d %d %d\n",
		Canon.ChannelBlackLevel[0], Canon.ChannelBlackLevel[1],
		Canon.ChannelBlackLevel[2], Canon.ChannelBlackLevel[3]);

	if (C.black)
	{
		fprintf(outfile, "\nblack: %d", C.black);
	}
	if (C.cblack[0] != 0)
	{
		fprintf(outfile, "\ncblack[0 .. 3]:");
		for (int c = 0; c < 4; c++)
			fprintf(outfile, " %d", C.cblack[c]);
	}
	if ((C.cblack[4] * C.cblack[5]) > 0)
	{
		fprintf(outfile, "\nBlackLevelRepeatDim: %d x %d\n", C.cblack[4], C.cblack[4]);
		int n = C.cblack[4] * C.cblack[5];
		fprintf(outfile, "cblack[6 .. %d]:", 6 + n - 1);
		for (int c = 6; c < 6 + n; c++)
			fprintf(outfile, " %d", C.cblack[c]);
	}

	if (C.linear_max[0] != 0)
	{
		fprintf(outfile, "\nHighlight linearity limits:");
		for (int c = 0; c < 4; c++)
			fprintf(outfile, " %ld", C.linear_max[c]);
	}

	if (P1.colors > 1)
	{
		if (C.cam_mul[0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'As shot' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %f", C.cam_mul[c]);
		}

		for (int cnt = 0; cnt < nEXIF_LightSources; cnt++)
		{
			if (C.WB_Coeffs[cnt][0] > 0)
			{
				fprintf(outfile, "\nMakernotes '%s' WB multipliers:", EXIF_LightSources[cnt]);
				for (int c = 0; c < 4; c++)
					fprintf(outfile, " %d", C.WB_Coeffs[cnt][c]);
			}
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Sunset][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Sunset' multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Sunset][c]);
		}

		if (C.WB_Coeffs[LIBRAW_WBI_Other][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Other' multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Other][c]);
		}

		if (C.WB_Coeffs[LIBRAW_WBI_Auto][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Camera Auto' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Auto][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Measured][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Camera Measured' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Measured][c]);
		}

		if (C.WB_Coeffs[LIBRAW_WBI_Custom][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom1][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom1' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom1][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom2][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom2' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom2][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom3][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom3' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom3][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom4][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom4' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom4][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom5][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom5' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom5][c]);
		}
		if (C.WB_Coeffs[LIBRAW_WBI_Custom6][0] > 0)
		{
			fprintf(outfile, "\nMakernotes 'Custom6' WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Custom6][c]);
		}

		if ((Nikon.ME_WB[0] != 0.0f) && (Nikon.ME_WB[0] != 1.0f))
		{
			fprintf(outfile, "\nNikon multi-exposure WB multipliers:");
			for (int c = 0; c < 4; c++)
				fprintf(outfile, " %f", Nikon.ME_WB[c]);
		}

		if (C.rgb_cam[0][0] > 0.0001)
		{
			fprintf(outfile, "\n\nCamera2RGB matrix (mode: %d):\n", MyCoolRawProcessor.imgdata.params.use_camera_matrix);
			for (int i = 0; i < P1.colors; i++)
				fprintf(outfile, "%6.4f\t%6.4f\t%6.4f\n", C.rgb_cam[i][0], C.rgb_cam[i][1],
				C.rgb_cam[i][2]);
		}

		fprintf(outfile, "\nXYZ->CamRGB matrix:\n");
		for (int i = 0; i < P1.colors; i++)
			fprintf(outfile, "%6.4f\t%6.4f\t%6.4f\n", C.cam_xyz[i][0], C.cam_xyz[i][1],
			C.cam_xyz[i][2]);

    for (int cnt = 0; cnt < 2; cnt++) {
      if (fabsf(C.P1_color[cnt].romm_cam[0]) > 0)
      {
        fprintf(outfile, "\nPhaseOne Matrix %d:\n", cnt+1);
        for (int i = 0; i < 3; i++)
          fprintf(outfile, "%6.4f\t%6.4f\t%6.4f\n", C.P1_color[cnt].romm_cam[i * 3],
          C.P1_color[cnt].romm_cam[i * 3 + 1],
          C.P1_color[cnt].romm_cam[i * 3 + 2]);
      }
    }

		if (fabsf(C.cmatrix[0][0]) > 0)
		{
			fprintf(outfile, "\ncamRGB -> sRGB Matrix:\n");
			for (int i = 0; i < P1.colors; i++)
			{
				for (int j = 0; j < P1.colors; j++)
					fprintf(outfile, "%6.4f\t", C.cmatrix[i][j]);
				fprintf(outfile, "\n");
			}
		}

		if (fabsf(C.ccm[0][0]) > 0)
		{
			fprintf(outfile, "\nColor Correction Matrix:\n");
			for (int i = 0; i < P1.colors; i++)
			{
				for (int j = 0; j < P1.colors; j++)
					fprintf(outfile, "%6.4f\t", C.ccm[i][j]);
				fprintf(outfile, "\n");
			}
		}

    for (int cnt = 0; cnt < 2; cnt++) {
      if (C.dng_color[cnt].illuminant != LIBRAW_WBI_None) {
        if (C.dng_color[cnt].illuminant < nEXIF_LightSources) {
          fprintf(outfile, "\nDNG Illuminant %d: %s",
            cnt + 1, EXIF_LightSources[C.dng_color[cnt].illuminant]);
        }
        else if (C.dng_color[cnt].illuminant == LIBRAW_WBI_Other) {
          fprintf(outfile, "\nDNG Illuminant %d: Other", cnt + 1);
        }
        else {
          fprintf(outfile, "\nDNG Illuminant %d is out of EXIF LightSources range [0:24, 255]: %d",
            cnt + 1, C.dng_color[cnt].illuminant);
        }
      }
    }

    for (int n=0; n<2; n++) {
      if (fabsf(C.dng_color[n].colormatrix[0][0]) > 0)
      {
        fprintf(outfile, "\nDNG color matrix %d:\n", n+1);
        for (int i = 0; i < P1.colors; i++)
          fprintf(outfile, "%6.4f\t%6.4f\t%6.4f\n", C.dng_color[n].colormatrix[i][0],
          C.dng_color[n].colormatrix[i][1],
          C.dng_color[n].colormatrix[i][2]);
      }
    }

    for (int n=0; n<2; n++) {
      if (fabsf(C.dng_color[n].calibration[0][0]) > 0)
      {
        fprintf(outfile, "\nDNG calibration matrix %d:\n", n+1);
        for (int i = 0; i < P1.colors; i++)
        {
          for (int j = 0; j < P1.colors; j++)
            fprintf(outfile, "%6.4f\t", C.dng_color[n].calibration[j][i]);
          fprintf(outfile, "\n");
        }
      }
    }

    for (int n=0; n<2; n++) {
      if (fabsf(C.dng_color[n].forwardmatrix[0][0]) > 0)
      {
        fprintf(outfile, "\nDNG forward matrix %d:\n", n+1);
        for (int i = 0; i < P1.colors; i++)
          fprintf(outfile, "%6.4f\t%6.4f\t%6.4f\n", C.dng_color[n].forwardmatrix[0][i],
          C.dng_color[n].forwardmatrix[1][i],
          C.dng_color[n].forwardmatrix[2][i]);
      }
    }

		fprintf(outfile, "\nDerived D65 multipliers:");
		for (int c = 0; c < P1.colors; c++)
			fprintf(outfile, " %f", C.pre_mul[c]);
	}
	fprintf(outfile, "\n");

	if (Sony.PixelShiftGroupID)
	{
		fprintf(outfile, "\nSony PixelShiftGroupPrefix 0x%x PixelShiftGroupID %d, ",
			Sony.PixelShiftGroupPrefix, Sony.PixelShiftGroupID);
		if (Sony.numInPixelShiftGroup)
		{
			fprintf(outfile, "shot# %d (starts at 1) of total %d\n",
				Sony.numInPixelShiftGroup, Sony.nShotsInPixelShiftGroup);
		}
		else
		{
			fprintf(outfile, "shots in PixelShiftGroup %d, already ARQ\n",
				Sony.nShotsInPixelShiftGroup);
		}
	}

	if (Sony.Sony0x9400_version)
		fprintf(outfile, "\nSONY Sequence data, tag 0x9400 version '%x'\n"
		"\tReleaseMode2: %d\n"
		"\tSequenceImageNumber: %d (starts at zero)\n"
		"\tSequenceLength1: %d shot(s)\n"
		"\tSequenceFileNumber: %d (starts at zero, exiftool starts at 1)\n"
		"\tSequenceLength2: %d file(s)\n",
		Sony.Sony0x9400_version, Sony.Sony0x9400_ReleaseMode2,
		Sony.Sony0x9400_SequenceImageNumber,
		Sony.Sony0x9400_SequenceLength1,
		Sony.Sony0x9400_SequenceFileNumber,
		Sony.Sony0x9400_SequenceLength2);
}