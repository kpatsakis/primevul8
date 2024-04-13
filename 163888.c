void Lab2ITU(const cmsCIELab* Lab, cmsUInt16Number Out[3])
{
	Out[0] = (cmsUInt16Number) floor((double) (Lab -> L / 100.)* 65535. );
	Out[1] = (cmsUInt16Number) floor((double) (Lab -> a / 170.)* 65535. + 32768. );
	Out[2] = (cmsUInt16Number) floor((double) (Lab -> b / 200.)* 65535. + 24576. );
}