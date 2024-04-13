void ModuleState::addConvertFloatToFloat(FormatCode input, FormatCode output)
{
	if (input == output)
		return;

	assert((input == kFloat && output == kDouble) ||
		(input == kDouble && output == kFloat));
	addModule(new ConvertFloat(input, output));
}