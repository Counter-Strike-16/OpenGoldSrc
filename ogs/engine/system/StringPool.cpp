/* <7fac3> ../engine/pr_edict.c:711 */
const char* EXT_FUNC SzFromIndex(int iString)
{
	return (const char *)(pr_strings + iString);
}

/* <7fc95> ../engine/pr_edict.c:794 */
int EXT_FUNC AllocEngineString(const char *szValue)
{
	return ED_NewString(szValue) - pr_strings;
}