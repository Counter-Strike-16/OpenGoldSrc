# Credits

### Thanks to:

* Id Software
* Valve Software
* ReHLDS Team

**Codebase:**

* ReHLDS as basis (aka GoldSrc's server-side aka modified NetQuake server-side)
* Client code from QWCL
* Some Quake2 improvements, bugfixes and render dll mechanics
* A bit of NetQuake if needed
* Some client side code from Xash3D
* Launcher code from MetaHook
* VGUI lib code from Nagi (CSBTE Team)
* Custom (re)implementation of VGUI2 library using some parts of code from CSBTE
* Dedicated launcher code from HLSDK 2.3 from AlliedModders
* GameUI implementation from CSBTE
* OpenGL render with additions from CSBTE
* Id Tech 4 (overall codebase improvements and multithreading support (in case we're going to impl it))
* Sound module will be split into it's own dll too (it will be used only when in client mode, so we'll load it when in non-dedicated mode using the single dll of the engine module for both client(play)/dedicated modes)