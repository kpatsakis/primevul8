void print_compactfun(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	trimSpaces(P1.make);
	trimSpaces(P1.model);
	trimSpaces(C.model2);
	trimSpaces(ShootingInfo.BodySerial);
	trimSpaces(ShootingInfo.InternalBodySerial);
	fprintf(outfile, "%s=%s", P1.make, P1.model);
	if (ShootingInfo.BodySerial[0] &&
		!(ShootingInfo.BodySerial[0] == 48 && !ShootingInfo.BodySerial[1]))
		fprintf(outfile, "=Body#: %s", ShootingInfo.BodySerial);
	else if (C.model2[0] && (!strncasecmp(P1.normalized_make, "Kodak", 5)))
		fprintf(outfile, "=Body#: %s", C.model2);
	if (ShootingInfo.InternalBodySerial[0])
		fprintf(outfile, "=Assy#: %s", ShootingInfo.InternalBodySerial);
	if (exifLens.LensSerial[0])
		fprintf(outfile, "=Lens#: %s", exifLens.LensSerial);
	if (exifLens.InternalLensSerial[0])
		fprintf(outfile, "=LensAssy#: %s", exifLens.InternalLensSerial);
	fprintf(outfile, "=\n");
}