#include "DeltaAPI.h"

extern "C"
{

#if !(OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL)
void Delta_SetField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_UnsetField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

int Delta_FindField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_SetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void Delta_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};
#else // OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL
void Delta_SetField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_UnsetField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_AddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

int Delta_FindField(struct delta_s *pFields, const char *fieldname)
{
};

void Delta_SetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void Delta_UnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};
#endif // OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL

}; // extern "C"