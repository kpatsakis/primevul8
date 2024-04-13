static void freeInstParams (AFPVu *values, int fileFormat)
{
	int	i;
	int	parameterCount = _af_units[fileFormat].instrumentParameterCount;

	for (i=0; i<parameterCount; i++)
	{
		if (_af_units[fileFormat].instrumentParameters[i].type == AU_PVTYPE_PTR)
			if (values[i].v != NULL)
				free(values[i].v);
	}

	free(values);
}