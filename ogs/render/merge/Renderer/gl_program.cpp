#include "gl_local.h"

int programs[MAX_PROGRAMS];
int numprograms;

void R_InitPrograms(void)
{
	numprograms = 0;
}

qboolean GL_IsProgramError(void)
{
	GLint iErrorPos, iIsNative;

	qglGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &iErrorPos);
	qglGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &iIsNative);

	if (iErrorPos != -1 || !iIsNative)
		return true;

	return false;
}

GLuint GL_CompileProgram(GLuint target, GLenum format, const GLvoid *string)
{
	GLuint program;
	qglGenProgramsARB(1, &program);
	qglEnable(target);
	qglBindProgramARB(target, program);
	qglProgramStringARB(target, format, strlen((const char *)string), string);
	qglDisable(target);

	if (GL_IsProgramError())
		return 0;

	programs[numprograms] = program;
	numprograms++;

	return program;
}

void R_FreePrograms(void)
{
	for (int i = 0; i < numprograms; i++)
		qglDeleteProgramsARB(1, (GLuint *)&programs[i]);
}