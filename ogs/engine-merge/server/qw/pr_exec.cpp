

typedef struct
{
	int				s;
	dfunction_t		*f;
} prstack_t;

#define	MAX_STACK_DEPTH		32
prstack_t	pr_stack[MAX_STACK_DEPTH];
int			pr_depth;

#define	LOCALSTACK_SIZE		2048
int			localstack[LOCALSTACK_SIZE];
int			localstack_used;


qboolean	pr_trace;
dfunction_t	*pr_xfunction;
int			pr_xstatement;


int		pr_argc;

char *pr_opnames[] =
{
};

char *PR_GlobalString (int ofs);
char *PR_GlobalStringNoContents (int ofs);


//=============================================================================

/*
=================
PR_PrintStatement
=================
*/
void PR_PrintStatement (dstatement_t *s)
{
	int		i;
	
	if ( (unsigned)s->op < sizeof(pr_opnames)/sizeof(pr_opnames[0]))
	{
		Con_Printf ("%s ",  pr_opnames[s->op]);
		i = strlen(pr_opnames[s->op]);
		for ( ; i<10 ; i++)
			Con_Printf (" ");
	}
		
	if (s->op == OP_IF || s->op == OP_IFNOT)
		Con_Printf ("%sbranch %i",PR_GlobalString(s->a),s->b);
	else if (s->op == OP_GOTO)
	{
		Con_Printf ("branch %i",s->a);
	}
	else if ( (unsigned)(s->op - OP_STORE_F) < 6)
	{
		Con_Printf ("%s",PR_GlobalString(s->a));
		Con_Printf ("%s", PR_GlobalStringNoContents(s->b));
	}
	else
	{
		if (s->a)
			Con_Printf ("%s",PR_GlobalString(s->a));
		if (s->b)
			Con_Printf ("%s",PR_GlobalString(s->b));
		if (s->c)
			Con_Printf ("%s", PR_GlobalStringNoContents(s->c));
	}
	Con_Printf ("\n");
}

/*
============
PR_StackTrace
============
*/
void PR_StackTrace (void)
{
	dfunction_t	*f;
	int			i;
	
	if (pr_depth == 0)
	{
		Con_Printf ("<NO STACK>\n");
		return;
	}
	
	pr_stack[pr_depth].f = pr_xfunction;
	for (i=pr_depth ; i>=0 ; i--)
	{
		f = pr_stack[i].f;
		
		if (!f)
		{
			Con_Printf ("<NO FUNCTION>\n");
		}
		else
			Con_Printf ("%12s : %s\n", PR_GetString(f->s_file), PR_GetString(f->s_name));		
	}
}


/*
============
PR_Profile_f

============
*/
void PR_Profile_f (void)
{
	dfunction_t	*f, *best;
	int			max;
	int			num;
	int			i;
	
	num = 0;	
	do
	{
		max = 0;
		best = NULL;
		for (i=0 ; i<progs->numfunctions ; i++)
		{
			f = &pr_functions[i];
			if (f->profile > max)
			{
				max = f->profile;
				best = f;
			}
		}
		if (best)
		{
			if (num < 10)
				Con_Printf ("%7i %s\n", best->profile, PR_GetString(best->s_name));
			num++;
			best->profile = 0;
		}
	} while (best);
}


/*
============
PR_RunError

Aborts the currently executing function
============
*/
void PR_RunError (char *error, ...)
{
	va_list		argptr;
	char		string[1024];

	va_start (argptr,error);
	vsprintf (string,error,argptr);
	va_end (argptr);

	PR_PrintStatement (pr_statements + pr_xstatement);
	PR_StackTrace ();
	Con_Printf ("%s\n", string);
	
	pr_depth = 0;		// dump the stack so Host_Error can shutdown functions

	Host_Error ("Program error");
}

/*
============================================================================
PR_ExecuteProgram

The interpretation main loop
============================================================================
*/

/*
====================
PR_EnterFunction

Returns the new program statement counter
====================
*/
int PR_EnterFunction (dfunction_t *f)
{
	int		i, j, c, o;

	pr_stack[pr_depth].s = pr_xstatement;
	pr_stack[pr_depth].f = pr_xfunction;	
	pr_depth++;
	if (pr_depth >= MAX_STACK_DEPTH)
		PR_RunError ("stack overflow");

// save off any locals that the new function steps on
	c = f->locals;
	if (localstack_used + c > LOCALSTACK_SIZE)
		PR_RunError ("PR_ExecuteProgram: locals stack overflow\n");

	for (i=0 ; i < c ; i++)
		localstack[localstack_used+i] = ((int *)pr_globals)[f->parm_start + i];
	localstack_used += c;

// copy parameters
	o = f->parm_start;
	for (i=0 ; i<f->numparms ; i++)
	{
		for (j=0 ; j<f->parm_size[i] ; j++)
		{
			((int *)pr_globals)[o] = ((int *)pr_globals)[OFS_PARM0+i*3+j];
			o++;
		}
	}

	pr_xfunction = f;
	return f->first_statement - 1;	// offset the s++
}

/*
====================
PR_LeaveFunction
====================
*/
int PR_LeaveFunction (void)
{
	int		i, c;

	if (pr_depth <= 0)
		Sys_Error ("prog stack underflow");

// restore locals from the stack
	c = pr_xfunction->locals;
	localstack_used -= c;
	if (localstack_used < 0)
		PR_RunError ("PR_ExecuteProgram: locals stack underflow\n");

	for (i=0 ; i < c ; i++)
		((int *)pr_globals)[pr_xfunction->parm_start + i] = localstack[localstack_used+i];

// up stack
	pr_depth--;
	pr_xfunction = pr_stack[pr_depth].f;
	return pr_stack[pr_depth].s;
}


/*
====================
PR_ExecuteProgram
====================
*/
void PR_ExecuteProgram (func_t fnum)
{
	eval_t	*a, *b, *c;
	int			s;
	dstatement_t	*st;
	dfunction_t	*f, *newf;
	int		runaway;
	int		i;
	edict_t	*ed;
	int		exitdepth;
	eval_t	*ptr;

	if (!fnum || fnum >= progs->numfunctions)
	{
		if (pr_global_struct->self)
			ED_Print (PROG_TO_EDICT(pr_global_struct->self));
		Host_Error ("PR_ExecuteProgram: NULL function");
	}
	
	f = &pr_functions[fnum];

	runaway = 100000;
	pr_trace = false;

// make a stack frame
	exitdepth = pr_depth;

	s = PR_EnterFunction (f);
	
while (1)
{
	s++;	// next statement

	st = &pr_statements[s];
	a = (eval_t *)&pr_globals[st->a];
	b = (eval_t *)&pr_globals[st->b];
	c = (eval_t *)&pr_globals[st->c];
	
	if (!--runaway) // if (--runaway == 0)
		PR_RunError ("runaway loop error");
		
	pr_xfunction->profile++;
	pr_xstatement = s;
	
	if (pr_trace)
		PR_PrintStatement (st);
		
	switch (st->op)
	{
	default:
		PR_RunError ("Bad opcode %i", st->op);
	}
}

}

/*----------------------*/

char *pr_strtbl[MAX_PRSTR];
int num_prstr;

char *PR_GetString(int num)
{
	if (num < 0) {
//Con_DPrintf("GET:%d == %s\n", num, pr_strtbl[-num]);
		return pr_strtbl[-num];
	}
	return pr_strings + num;
}

int PR_SetString(char *s)
{
	int i;

	if (s - pr_strings < 0) {
		for (i = 0; i <= num_prstr; i++)
			if (pr_strtbl[i] == s)
				break;
		if (i < num_prstr)
			return -i;
		if (num_prstr == MAX_PRSTR - 1)
			Sys_Error("MAX_PRSTR");
		num_prstr++;
		pr_strtbl[num_prstr] = s;
//Con_DPrintf("SET:%d == %s\n", -num_prstr, s);
		return -num_prstr;
	}
	return (int)(s - pr_strings);
}

