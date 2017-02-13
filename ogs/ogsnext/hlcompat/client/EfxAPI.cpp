efx_api_t gEfxApi =
{
	CL_AllocParticle,
	CL_BlobExplosion,
	CL_Blood,
	CL_BloodSprite,
	CL_BloodStream,
	CL_BreakModel,
	CL_Bubbles,
	CL_BubbleTrail,
	CL_BulletImpactParticles,
	CL_EntityParticles,
	CL_Explosion,
	CL_FizzEffect,
	CL_FireField,
	CL_FlickerParticles,
	CL_FunnelSprite,
	CL_Implosion,
	CL_Large_Funnel,
	CL_LavaSplash,
	CL_MultiGunshot,
	CL_MuzzleFlash,
	CL_ParticleBox,
	CL_ParticleBurst,
	CL_ParticleExplosion,
	CL_ParticleExplosion2,
	CL_ParticleLine,
	CL_PlayerSprites,
	CL_Projectile,
	CL_RicochetSound,
	CL_RicochetSprite,
	CL_RocketFlare,
	CL_RocketTrail,
	CL_RunParticleEffect,
	CL_ShowLine,
	CL_SparkEffect,
	CL_SparkShower,
	CL_SparkStreaks,
	CL_Spray,
	CL_Sprite_Explode,
	CL_Sprite_Smoke,
	CL_Sprite_Spray,
	CL_Sprite_Trail,
	CL_Sprite_WallPuff,
	CL_StreakSplash,
	CL_TracerEffect,
	CL_UserTracerParticle,
	CL_TracerParticles,
	CL_TeleportSplash,
	CL_TempSphereModel,
	CL_TempModel,
	CL_DefaultSprite,
	CL_TempSprite,
	CL_DecalIndex,
	CL_DecalIndexFromName,
	CL_DecalShoot,
	CL_AttachTentToPlayer,
	CL_KillAttachedTents,
	CL_BeamCirclePoints,
	CL_BeamEntPoint,
	CL_BeamEnts,
	CL_BeamFollow,
	CL_BeamKill,
	CL_BeamLightning,
	CL_BeamPoints,
	CL_BeamRing,
	CL_AllocDlight,
	CL_AllocElight,
	CL_TempEntAlloc,
	CL_TempEntAllocNoModel,
	CL_TempEntAllocHigh,
	CL_TempEntAllocCustom,
	CL_GetPackedColor,
	CL_LookupColor,
	CL_DecalRemoveAll,
	CL_FireCustomDecal
};

/*
==============
CL_BloodSprite

Create a high priority blood sprite
and some blood drops. This is high-priority tent
==============
*/
void CL_BloodSprite( const vec3_t org, int colorIndex, int modelIndex, int modelIndex2, float size )
{
	TEMPENTITY	*pTemp;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	// large, single blood sprite is a high-priority tent
	if(( pTemp = CL_TempEntAllocHigh( org, Mod_Handle( modelIndex ))) != NULL )
	{
		int	i, frameCount;
		vec3_t	offset, dir;
		vec3_t	forward, right, up;

		colorIndex = bound( 0, colorIndex, 256 );

		Mod_GetFrames( modelIndex, &frameCount );
		pTemp->entity.curstate.rendermode = kRenderTransTexture;
		pTemp->entity.curstate.renderfx = kRenderFxClampMinScale;
		pTemp->entity.curstate.scale = Com_RandomFloat(( size / 25.0f ), ( size / 35.0f ));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = clgame.palette[colorIndex][0];
		pTemp->entity.curstate.rendercolor.g = clgame.palette[colorIndex][1];
		pTemp->entity.curstate.rendercolor.b = clgame.palette[colorIndex][2];
		pTemp->entity.curstate.framerate = frameCount * 4; // Finish in 0.250 seconds
		// play the whole thing once
		pTemp->die = cl.time + (frameCount / pTemp->entity.curstate.framerate);

		pTemp->entity.angles[2] = Com_RandomLong( 0, 360 );
		pTemp->bounceFactor = 0;

		VectorSet( forward, 0.0f, 0.0f, 1.0f );	// up-vector
		VectorVectors( forward, right, up );

		Mod_GetFrames( modelIndex2, &frameCount );

		// create blood drops
		for( i = 0; i < 14; i++ )
		{
			// originate from within a circle 'scale' inches in diameter.
			VectorCopy( org, offset );
			VectorMA( offset, Com_RandomFloat( -0.5f, 0.5f ) * size, right, offset ); 
			VectorMA( offset, Com_RandomFloat( -0.5f, 0.5f ) * size, up, offset ); 

			pTemp = CL_TempEntAlloc( org, Mod_Handle( modelIndex2 ));
			if( !pTemp ) return;

			pTemp->flags = FTENT_COLLIDEWORLD|FTENT_SLOWGRAVITY;

			pTemp->entity.curstate.rendermode = kRenderTransTexture;
			pTemp->entity.curstate.renderfx = kRenderFxClampMinScale; 
			pTemp->entity.curstate.scale = Com_RandomFloat(( size / 25.0f), ( size / 35.0f ));
			pTemp->entity.curstate.rendercolor.r = clgame.palette[colorIndex][0];
			pTemp->entity.curstate.rendercolor.g = clgame.palette[colorIndex][1];
			pTemp->entity.curstate.rendercolor.b = clgame.palette[colorIndex][2];
			pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
			pTemp->die = cl.time + Com_RandomFloat( 1.0f, 3.0f );

			pTemp->entity.angles[2] = Com_RandomLong( 0, 360 );
			pTemp->bounceFactor = 0;

			dir[0] = forward[0] + Com_RandomFloat( -0.8f, 0.8f );
			dir[1] = forward[1] + Com_RandomFloat( -0.8f, 0.8f );
			dir[2] = forward[2];

			VectorScale( dir, Com_RandomFloat( 8.0f * size, 20.0f * size ), pTemp->entity.baseline.origin );
			pTemp->entity.baseline.origin[2] += Com_RandomFloat( 4.0f, 16.0f ) * size;
		};
	};
};

/*
==============
CL_Bubbles

Create bubbles
==============
*/
void CL_Bubbles( const vec3_t mins, const vec3_t maxs, float height, int modelIndex, int count, float speed )
{
	TEMPENTITY	*pTemp;
	int		i, frameCount;
	float		sine, cosine, zspeed;
	float		angle;
	vec3_t		origin;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	Mod_GetFrames( modelIndex, &frameCount );

	for ( i = 0; i < count; i++ )
	{
		origin[0] = Com_RandomLong( mins[0], maxs[0] );
		origin[1] = Com_RandomLong( mins[1], maxs[1] );
		origin[2] = Com_RandomLong( mins[2], maxs[2] );
		pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];
		angle = Com_RandomLong( -M_PI, M_PI );
		SinCos( angle, &sine, &cosine );
		
		zspeed = Com_RandomLong( 80, 140 );
		VectorSet( pTemp->entity.baseline.origin, speed * cosine, speed * sine, zspeed );
		pTemp->die = cl.time + ((height - (origin[2] - mins[2])) / zspeed) - 0.1f;
		pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		
		// Set sprite scale
		pTemp->entity.curstate.scale = 1.0f / Com_RandomFloat( 4.0f, 16.0f );
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 192; // g-cont. why difference with FizzEffect ???		
	};
};

/*
==============
CL_BubbleTrail

Create bubble trail
==============
*/
void CL_BubbleTrail( const vec3_t start, const vec3_t end, float flWaterZ, int modelIndex, int count, float speed )
{
	TEMPENTITY	*pTemp;
	int		i, frameCount;
	float		dist, angle, zspeed;
	vec3_t		origin;

	if( Mod_GetType( modelIndex ) == mod_bad )
		return;

	Mod_GetFrames( modelIndex, &frameCount );

	for( i = 0; i < count; i++ )
	{
		dist = Com_RandomFloat( 0, 1.0 );
		VectorLerp( start, dist, end, origin );
		pTemp = CL_TempEntAlloc( origin, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];
		angle = Com_RandomLong( -M_PI, M_PI );

		zspeed = Com_RandomLong( 80, 140 );
		VectorSet( pTemp->entity.baseline.origin, speed * cos( angle ), speed * sin( angle ), zspeed );
		pTemp->die = cl.time + ((flWaterZ - (origin[2] - start[2])) / zspeed) - 0.1f;
		pTemp->entity.curstate.frame = Com_RandomLong( 0, frameCount - 1 );
		// Set sprite scale
		pTemp->entity.curstate.scale = 1.0 / Com_RandomFloat( 4, 8 );
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = 192;
	};
};

/*
===============
CL_Sprite_Explode

apply params for exploding sprite
===============
*/
void CL_Sprite_Explode( TEMPENTITY *pTemp, float scale, int flags )
{
	if( !pTemp ) return;

	if( flags & TE_EXPLFLAG_NOADDITIVE )
	{
		// solid sprite
		pTemp->entity.curstate.rendermode = kRenderNormal;
		pTemp->entity.curstate.renderamt = 255; 
	}
	else if( flags & TE_EXPLFLAG_DRAWALPHA )
	{
		// alpha sprite (came from hl2)
		pTemp->entity.curstate.rendermode = kRenderTransAlpha;
		pTemp->entity.curstate.renderamt = 180;
	}
	else
	{
		// additive sprite
		pTemp->entity.curstate.rendermode = kRenderTransAdd;
		pTemp->entity.curstate.renderamt = 120;
	}

	if( flags & TE_EXPLFLAG_ROTATE )
		pTemp->entity.angles[2] = Com_RandomLong( 0, 360 ); // came from hl2

	pTemp->entity.curstate.renderfx = kRenderFxNone;
	pTemp->entity.baseline.origin[2] = 8;
	pTemp->entity.origin[2] += 10;
	pTemp->entity.curstate.scale = scale;
};

/*
===============
CL_Sprite_Smoke

apply params for smoke sprite
===============
*/
void CL_Sprite_Smoke( TEMPENTITY *pTemp, float scale )
{
	int	iColor;

	if( !pTemp )
		return;

	iColor = Com_RandomLong( 20, 35 );
	pTemp->entity.curstate.rendermode = kRenderTransAlpha;
	pTemp->entity.curstate.renderfx = kRenderFxNone;
	pTemp->entity.baseline.origin[2] = 30;
	pTemp->entity.curstate.rendercolor.r = iColor;
	pTemp->entity.curstate.rendercolor.g = iColor;
	pTemp->entity.curstate.rendercolor.b = iColor;
	pTemp->entity.origin[2] += 20;
	pTemp->entity.curstate.scale = scale;
};

/*
===============
CL_Sprite_Spray

Spray of alpha sprites
===============
*/
void CL_Sprite_Spray( const vec3_t pos, const vec3_t dir, int modelIndex, int count, int speed, int iRand )
{
	CL_Spray( pos, dir, modelIndex, count, speed, iRand, kRenderTransAlpha );
}

/*
===============
CL_Sprite_Trail

Line of moving glow sprites with gravity,
fadeout, and collisions
===============
*/
void CL_Sprite_Trail( int type, const vec3_t vecStart, const vec3_t vecEnd, int modelIndex, int nCount, float flLife, float flSize, float flAmplitude, int nRenderamt, float flSpeed )
{
	TEMPENTITY	*pTemp;
	vec3_t		vecDelta, vecDir;
	int		i, flFrameCount;

	if( Mod_GetType( modelIndex ) == mod_bad )
	{
		MsgDev( D_INFO, "No model %d!", modelIndex );
		return;
	}	

	Mod_GetFrames( modelIndex, &flFrameCount );

	VectorSubtract( vecEnd, vecStart, vecDelta );
	VectorNormalize2( vecDelta, vecDir );

	flAmplitude /= 256.0f;

	for( i = 0; i < nCount; i++ )
	{
		vec3_t	vecPos, vecVel;

		// Be careful of divide by 0 when using 'count' here...
		if( i == 0 ) VectorCopy( vecStart, vecPos );
		else VectorMA( vecStart, ( i / ( nCount - 1.0f )), vecDelta, vecPos );

		pTemp = CL_TempEntAlloc( vecPos, Mod_Handle( modelIndex ));
		if( !pTemp ) return;

		pTemp->flags = (FTENT_COLLIDEWORLD|FTENT_SPRCYCLE|FTENT_FADEOUT|FTENT_SLOWGRAVITY);

		VectorScale( vecDir, flSpeed, vecVel );
		vecVel[0] += Com_RandomFloat( -127.0f, 128.0f ) * flAmplitude;
		vecVel[1] += Com_RandomFloat( -127.0f, 128.0f ) * flAmplitude;
		vecVel[2] += Com_RandomFloat( -127.0f, 128.0f ) * flAmplitude;
		VectorCopy( vecVel, pTemp->entity.baseline.origin );
		VectorCopy( vecPos, pTemp->entity.origin );

		pTemp->entity.curstate.scale = flSize;
		pTemp->entity.curstate.rendermode = kRenderGlow;
		pTemp->entity.curstate.renderfx = kRenderFxNoDissipation;
		pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt = nRenderamt;

		pTemp->entity.curstate.frame = Com_RandomLong( 0, flFrameCount - 1 );
		pTemp->frameMax = flFrameCount - 1;
		pTemp->die = cl.time + flLife + Com_RandomFloat( 0.0f, 4.0f );
	}
};