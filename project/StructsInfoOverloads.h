#pragma once
#include <Exam_HelperStructs.h>

#include "EDecisionMaking.h"

struct HouseInfoExpanded : HouseInfo
{
	HouseInfoExpanded(HouseInfo Original) :
		HouseInfo(Original){};
	
	bool doorsChecked = false;
	bool CenterVisited = false;
	float lastTimeVisited = 0.f;
	bool BottemDoor = false,
		TopDoor = false,
		LeftDoor = false,
		RightDoor = false;
	
	float TimeLastChecked = 0.f;
	
};

struct AgentActions
{
	bool CanRun = false; //
	bool GrabItem = false; //
	bool UseItem = false; //
	int ItemToUse = -1;
	bool RemoveItem = false; //
	bool Shoot = false;
	float AngSpeed = 0.f; //
	float ShootDelay = 0.f;
	int ShotToFire = 0;

	bool RecharingStamina = false;
	bool ShouldRun = false;
	
};

struct ExpandedItemInfo
{
	ItemInfo itemInfo;
	int uses = 0;
};