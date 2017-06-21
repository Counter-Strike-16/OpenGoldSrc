//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "vprof.h"

int CVProfNode::s_iCurrentUniqueNodeID = 0;

CVProfile g_VProfCurrentProfile;

CVProfNode *CVProfNode::GetSubNode( const tchar *pszName, int detailLevel, const tchar *pBudgetGroupName, int budgetFlags )
{
	//TODO: implement - Solokiller
	return nullptr;
}

CVProfNode *CVProfNode::GetSubNode( const tchar *pszName, int detailLevel, const tchar *pBudgetGroupName )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void CVProfNode::MarkFrame()
{
	//TODO: implement - Solokiller
}

void CVProfNode::ResetPeak()
{
	//TODO: implement - Solokiller
}

void CVProfNode::Pause()
{
}

void CVProfNode::Resume()
{
	//TODO: implement - Solokiller
}

void CVProfNode::Reset()
{
	//TODO: implement - Solokiller
}

void CVProfNode::EnterScope()
{
	//TODO: implement - Solokiller
}

bool CVProfNode::ExitScope()
{
	//TODO: implement - Solokiller
	return true;
}

CVProfile::CVProfile()
	: m_Root( _T( "Root" ), 0, nullptr, "Unaccounted", 0 )
	, m_fAtRoot( true )
{
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, 6 );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_WORLD_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_DISPLACEMENT_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_GAME, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_NPCS, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_SERVER_ANIM, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_CLIENT_ANIMATION, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_PHYSICS, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_STATICPROP_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_MODEL_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_LIGHTCACHE, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_BRUSHMODEL_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_SHADOW_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_DETAILPROP_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_PARTICLE_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_ROPES, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_DLIGHT_RENDERING, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OTHER_NETWORKING, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OTHER_SOUND, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OTHER_VGUI, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OTHER_FILESYSTEM, BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_PREDICTION, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_INTERPOLATION, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_SWAP_BUFFERS, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( _T( "AINet" ), BUDGETFLAG_SERVER | BUDGETFLAG_OTHER );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OCCLUSION, BUDGETFLAG_CLIENT );
	BudgetGroupNameToBudgetGroupID( VPROF_BUDGETGROUP_OVERLAYS, BUDGETFLAG_CLIENT );
}

CVProfile::~CVProfile()
{
	Term();
}

void CVProfile::Term()
{
}

int CVProfile::BudgetGroupNameToBudgetGroupID( const tchar *pBudgetGroupName )
{
	//TODO: implement - Solokiller
	return 0;
}

int CVProfile::BudgetGroupNameToBudgetGroupID( const tchar *pBudgetGroupName, int budgetFlagsToORIn )
{
	//TODO: implement - Solokiller
	return 0;
}
