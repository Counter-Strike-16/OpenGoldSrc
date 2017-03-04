

extern	byte		gammatable[256];	// palette is sent through this
extern	byte		ramps[3][256];
extern float v_blend[4];

float V_CalcRoll (vec3_t angles, vec3_t velocity);