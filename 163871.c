void ITU2Lab(const cmsUInt16Number In[3], cmsCIELab* Lab)
{
	Lab -> L = (double) In[0] / 655.35;
	Lab -> a = (double) 170.* (In[1] - 32768.) / 65535.;
	Lab -> b = (double) 200.* (In[2] - 24576.) / 65535.;
}