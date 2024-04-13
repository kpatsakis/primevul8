bool read(AFvirtualfile *vf, T *value)
{
	return vf->read(value, sizeof (*value)) == sizeof (*value);
}