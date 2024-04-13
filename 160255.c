void print_szfun(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	id2hr_t *Aspect;
	fprintf(outfile, "%s\t%s\t%s\t%d\t%d\n", fn.c_str(), P1.make, P1.model, S.width,
		S.height);
	/*
	fprintf(outfile, "\n%s\t%s\t%s\n", filelist[i].c_str(), P1.make, P1.model);
	fprintf(outfile, "\tCoffin:\traw %dx%d mrg %d %d img %dx%d\n", S.raw_width,
	S.raw_height, S.left_margin, S.top_margin, S.width, S.height); printf
	("\tmnote: \t"); if (Canon.SensorWidth) printf ("sensor %dx%d ",
	Canon.SensorWidth, Canon.SensorHeight); if (Nikon.SensorWidth) printf
	("sensor %dx%d ", Nikon.SensorWidth, Nikon.SensorHeight);
	fprintf(outfile, "inset: start %d %d img %dx%d aspect calc: %f Aspect in file:
	", S.raw_inset_crop.cleft, S.raw_inset_crop.ctop,
	S.raw_inset_crop.cwidth, S.raw_inset_crop.cheight,
	((float)S.raw_inset_crop.cwidth /
	(float)S.raw_inset_crop.cheight)); Aspect =
	lookup_id2hr(S.raw_inset_crop.aspect, AspectRatios, nAspectRatios); if
	(Aspect) printf ("%s\n", Aspect->name); else printf ("Other %d\n",
	S.raw_inset_crop.aspect);

	if (Nikon.SensorHighSpeedCrop.cwidth) {
	fprintf(outfile, "\tHighSpeed crop from: %d x %d at top left pixel: (%d,
	%d)\n", Nikon.SensorHighSpeedCrop.cwidth,
	Nikon.SensorHighSpeedCrop.cheight, Nikon.SensorHighSpeedCrop.cleft,
	Nikon.SensorHighSpeedCrop.ctop);
	}
	*/
}