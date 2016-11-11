#pragma once

extern "C"
{

#if !(OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL)
void EngFunc_DeltaSetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaAddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

int EngFunc_DeltaFindField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void EngFunc_DeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};
#else // OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL
void EngFunc_DeltaSetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaAddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
};

int EngFunc_DeltaFindField(struct delta_s *pFields, const char *fieldname)
{
};

void EngFunc_DeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};

void EngFunc_DeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
};
#endif // OGS_NULL_DELTA_API_IMPL || OGS_NULL_EXPORT_API_IMPL

};