#ifndef GAMEUI_CAREERPROFILE_H
#define GAMEUI_CAREERPROFILE_H

#include <string>
#include <vector>

struct SavedMap
{
	std::string name;
	bool defeated;
};

typedef std::vector<std::string> StringVector;
typedef std::vector<SavedMap> SavedMapVector;

class CCareerProfileDifficultyData
{
public:
	bool played;
	int points;
	StringVector hired;
	SavedMapVector maps;
	std::string lastMap;
	int pointsFlashed;

	void Reset();
};

class CCareerProfileData
{
public:
	int filetime;
	std::string name;
	int sliderPos;

	CareerDifficultyType lastDifficulty;

	bool medals[ MAX_CAREER_DIFFICULTY ];
	CCareerProfileDifficultyData difficulty[ MAX_CAREER_DIFFICULTY ];

	int tutorData[ 256 ];

	void Reset();
};

#endif //GAMEUI_CAREERPROFILE_H
