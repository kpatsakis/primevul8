bool write(AFvirtualfile *vf, const T *value)
{
	return vf->write(value, sizeof (*value)) == sizeof (*value);
}