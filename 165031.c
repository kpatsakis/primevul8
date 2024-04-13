void ModuleState::addConvertIntToInt(FormatCode input, FormatCode output)
{
	if (input == output)
		return;

	assert(isInteger(input));
	assert(isInteger(output));
	addModule(new ConvertInt(input, output));
}