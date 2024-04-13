getDebugLevel(const char *variableName, unsigned long defaultDebugLevel) {
  const char *const valueOrNull = getenv(variableName);
  if (valueOrNull == NULL) {
    return defaultDebugLevel;
  }
  const char *const value = valueOrNull;

  errno = 0;
  char *afterValue = (char *)value;
  unsigned long debugLevel = strtoul(value, &afterValue, 10);
  if ((errno != 0) || (afterValue[0] != '\0')) {
    errno = 0;
    return defaultDebugLevel;
  }

  return debugLevel;
}