//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_ConfigWizard.h"
#include "VGUI_TreeFolder.h"
#include "VGUI_LineBorder.h"
#include "VGUI_LoweredBorder.h"
#include "VGUI_Button.h"

using namespace vgui;

ConfigWizard::ConfigWizard(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
	setBorder(new LineBorder());

	_treeFolder=new TreeFolder("DonkeyFoo");
	_treeFolder->setParent(this);
	_treeFolder->setBorder(new LoweredBorder());

	_client=new Panel(80,30,64,64);
	_client->setParent(this);
	_treeFolder->setBorder(new LineBorder());

	_okButton=new Button("Ok",80,80,60,20);
	_okButton->setParent(this);

	_cancelButton=new Button("Cancel",120,80);
	_cancelButton->setParent(this);

	_applyButton=new Button("Apply",160,80);
	_applyButton->setParent(this);

	_helpButton=new Button("Help",200,80);
	_helpButton->setParent(this);
}

void ConfigWizard::setSize(int wide,int tall)
{
	setSize(wide,tall);
	getPaintSize(wide,tall);

	_treeFolder->setBounds(10,10,160,tall-60);
	_client->setBounds(180,10,wide-190,tall-60);
	_helpButton->setPos(wide-80,tall-25);
	_applyButton->setPos(wide-150,tall-25);
	_cancelButton->setPos(wide-220,tall-25);
	_okButton->setPos(wide-290,tall-25);
}

Panel* ConfigWizard::getClient()
{
	return _client;
}

TreeFolder* ConfigWizard::getFolder()
{
	return _treeFolder;
}
