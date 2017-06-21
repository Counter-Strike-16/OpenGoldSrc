// IVoiceTweak implementation

IVoiceTweak gVoiceApi =
{
	Voice_StartVoiceTweakMode,
	Voice_EndVoiceTweakMode,
	Voice_SetControlFloat,
	Voice_GetControlFloat,
	Voice_GetSpeakingVolume
};

static int Voice_StartVoiceTweakMode()
{
	// TODO: implement
	return 0;
};

static void Voice_EndVoiceTweakMode()
{
	// TODO: implement
};

static void Voice_SetControlFloat(VoiceTweakControl aeControl, float afValue)
{
	// TODO: implement
};

static float Voice_GetControlFloat(VoiceTweakControl aeControl)
{
	// TODO: implement
	return 1.0f;
};

static int Voice_GetSpeakingVolume()
{
	// TODO: implement
	return 255;
};