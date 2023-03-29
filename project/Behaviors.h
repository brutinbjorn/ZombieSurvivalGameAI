#pragma once
#include "EBehaviorTree.h"
#include "PlayerAgent.h"
#include "StructsInfoOverloads.h"


inline Elite::BehaviorState ChangeToFlee(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackBoard->GetData("Target", target);

	if(!pAgent)
	{
		return Elite::Failure;
	}
	pAgent->SetToFlee(target);
	return Elite::Success;
}

inline Elite::BehaviorState ChangeToWander(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);

	if(!pAgent)
	{
		return Elite::BehaviorState::Failure;
	}
	
	pAgent->SetToWander();
	return Elite::BehaviorState::Success;
}

inline Elite::BehaviorState ChangeToSeek(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackBoard->GetData("Target", target);
	
	if (!pAgent)
	{
		return Elite::BehaviorState::Failure;
	}

	std::cout << "moving to" << std::endl;
	pAgent->SetToSeek(target);
	return Elite::BehaviorState::Success;
}

inline Elite::BehaviorState SeekItem(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackBoard->GetData("Target", target);

	if (!pAgent)
	{
		return Elite::BehaviorState::Failure;
	}

	std::cout << "moving to" << std::endl;
	pAgent->SetToSeek(target);
	return Elite::BehaviorState::Success;
}

inline Elite::BehaviorState GrabItem(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackBoard->GetData("Target", target);
	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	
	if (!pAgent || !pActions)
		//pBlackBoard->ChangeData("PickupStuff", false);
		//pAgent->SetPickUpItem(false);
		return Elite::BehaviorState::Failure;
	

	if(Elite::Distance(pAgent->GetAgentInfo().Position , target) < pAgent->GetAgentInfo().GrabRange)
	{
		pActions->GrabItem = true;
		return Elite::BehaviorState::Success;
	}
	
	pActions->GrabItem = false;
	return Elite::BehaviorState::Running;
	
}

inline Elite::BehaviorState FaceEnemy(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target = Elite::Vector2();
	pBlackBoard->GetData("FaceTarget", target);
	std::vector<EnemyInfo>* pEnemys;
	pBlackBoard->GetData("EnemysInFOV", pEnemys);
	
	if(!pAgent || !pEnemys || pEnemys->empty())
	{
		if(pAgent)
		pAgent->SetToFace(false, {});
		return Elite::Failure;
	}
	
	EnemyInfo closestEnemy = (*pEnemys)[0];
	float ClosestDistanceSqr =
		Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pEnemys)[0].Location);

	for (size_t i = 0; i < pEnemys->size(); ++i)
	{
		const EnemyInfo& item = (*pEnemys)[i];
		Elite::Vector2 agentToItem = (*pEnemys)[i].Location - pAgent->GetAgentInfo().Position;

		float CurrDistanceSqr = Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pEnemys)[i].Location);
		if (CurrDistanceSqr < ClosestDistanceSqr)
		{
			closestEnemy = (*pEnemys)[i];
			ClosestDistanceSqr = CurrDistanceSqr;

		}
	}
	pAgent->SetToFace(true, closestEnemy.Location);

	if (ClosestDistanceSqr * ClosestDistanceSqr < 1.f)
		pAgent->SetToFlee(closestEnemy.Location);
	
	return Elite::Success;
	
}

inline Elite::BehaviorState LookBackwards(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target;
	//pBlackBoard->GetData("FaceTarget", target);
	
	if (!pAgent || !pBlackBoard->GetData("FaceTarget", target))
	{
		
		return Elite::Failure;
	}

	//target = pAgent->GetAgentInfo().Position - pAgent->GetAgentInfo().LinearVelocity * 5;
	
	pAgent->SetToFace(true, target);
	return Elite::Success;
	
	
}

inline Elite::BehaviorState UseGun(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<EnemyInfo>* pEnemys;
	pBlackBoard->GetData("EnemysInFOV", pEnemys);
	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	if (!pAgent || !pEnemys || pEnemys->empty() || !pActions)
	{
		if (pActions)
		{
			pActions->UseItem = false;
			pActions->Shoot = false;
		}
		return Elite::Failure;
	}

	pActions->Shoot = true;
	pActions->UseItem = true;
	return Elite::Success;
	
}

inline Elite::BehaviorState StartRunning(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);

	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	if (!pAgent || !pActions)
	{
		if (pActions)
		{
			pActions->CanRun = false;
		}
		return Elite::Failure;
	}

	pActions->CanRun = true;
	return Elite::Success;
}

inline Elite::BehaviorState ShouldRun(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	if (!pAgent || !pActions)
		return Elite::Failure;

	float stamina = pAgent->GetAgentInfo().Stamina;
	if(stamina > 9.f)
	{
		pActions->RecharingStamina = false;
	}
	else if(stamina < 2.f)
	{
		pActions->RecharingStamina = true;
	}

	if (!pActions->RecharingStamina)
		pActions->ShouldRun = true;

	return Elite::Success;
	
}

inline bool NewHouseInFOV(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<HouseInfoExpanded>* pHouseVec = nullptr;
	pBlackBoard->GetData("HouseVec",pHouseVec);

	if (!pAgent || !pHouseVec || pHouseVec->size() < 1)
		return false;

	for (size_t i = 0; i < pHouseVec->size(); ++i)
	{
		const HouseInfoExpanded& house = (*pHouseVec)[i];
		if (!house.CenterVisited)
		{
			pBlackBoard->ChangeData("Target", house.Center);
			return true;
		}
	}

	return false;
}

inline bool NewItemInFOV(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<ItemInfo>* pItemVec = nullptr;
	pBlackBoard->GetData("ItemsInFOV", pItemVec);
	
	if (!pAgent || !pItemVec || pItemVec->size() < 1)
		return false;


	ItemInfo* closestItem = &(*pItemVec)[0];
	float ClosestDistanceSqr = 
		Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pItemVec)[0].Location);

	for (size_t i = 0; i < pItemVec->size(); ++i )
	{
		const ItemInfo& item = (*pItemVec)[i];
		Elite::Vector2 agentToItem = (*pItemVec)[i].Location - pAgent->GetAgentInfo().Position;

		float CurrDistanceSqr = Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pItemVec)[i].Location);
		if (CurrDistanceSqr < ClosestDistanceSqr)
		{
			closestItem = &(*pItemVec)[i];
			ClosestDistanceSqr = CurrDistanceSqr;

		}	
	}

	pBlackBoard->ChangeData("Target", closestItem->Location);
	pBlackBoard->ChangeData("ItemTarget", closestItem);
	return true;
		
	//return false;
}

inline bool NewPurgeZoneInFOV(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<PurgeZoneInfo>* pItemVec = nullptr;
	pBlackBoard->GetData("PurgeZonesInFOV", pItemVec);

	if (!pAgent || !pItemVec || pItemVec->empty())
		return false;
	
	PurgeZoneInfo* closestItem = &(*pItemVec)[0];
	float ClosestDistanceSqr =
		Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pItemVec)[0].Center);
	
	for (size_t i = 0; i < pItemVec->size(); ++i)
	{
		const PurgeZoneInfo& item = (*pItemVec)[i];
		Elite::Vector2 agentToItem = (*pItemVec)[i].Center - pAgent->GetAgentInfo().Position;

		float CurrDistanceSqr = Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pItemVec)[i].Center);
		if (CurrDistanceSqr < ClosestDistanceSqr && agentToItem.Magnitude() <= item.Radius + 2)
		{
			closestItem = &(*pItemVec)[i];
			ClosestDistanceSqr = CurrDistanceSqr;
		}
	}
	pBlackBoard->ChangeData("Target", closestItem->Center);
	//pBlackBoard->ChangeData("ItemTarget", closestItem);
	return true;

}

inline bool ShouldGrabItem(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<ItemInfo>* pItemVec = nullptr;
	pBlackBoard->GetData("ItemsInFOV", pItemVec);

	std::vector<ExpandedItemInfo>* pItemInventory = nullptr;
	pBlackBoard->GetData("ItemsInInventory", pItemInventory);

	ItemInfo* target = nullptr;
	pBlackBoard->GetData("ItemTarget", target);
	
	if (!pAgent || !pItemVec || pItemVec->size() < 1 || !pItemInventory || !target)
		return false;

	if (pItemInventory->empty())
		return true;

	int gunsInInventory = 0;
	int foodInInventory = 0;
	int healsInINventory = 0;

	for(size_t i = 0; i < pItemInventory->size(); i++)
	{
		switch ((*pItemInventory)[i].itemInfo.Type)
		{
		case eItemType::PISTOL:
			gunsInInventory++;
			break;
		case eItemType::MEDKIT:
			healsInINventory++;
			break;
		case eItemType::FOOD:
			foodInInventory++;
			break;
		case eItemType::GARBAGE: 
			break;
		case eItemType::RANDOM_DROP: 
			break;
		case eItemType::RANDOM_DROP_WITH_CHANCE: 
			break;
		default:
			break;
			;
		};
	}


	bool shouldGrabItem = true;
	switch (target->Type)
	{
	case eItemType::PISTOL:
		if (gunsInInventory >= 2)
			shouldGrabItem = false;
		break;
	case eItemType::MEDKIT:
		if (healsInINventory >= 2)
			shouldGrabItem = false;
		break;
	case eItemType::FOOD:
		if(foodInInventory >= 1)
			shouldGrabItem = false;
		break;
	default:
		return false;
		break;
	};
	
	return shouldGrabItem;
	
}

inline bool NewPointOnFarSight(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<Elite::Vector2>* pSightVec = nullptr;
	pBlackBoard->GetData("FarSightVec", pSightVec);

	if (!pAgent || !pSightVec || pSightVec->size() < 1)
		return false;

	Elite::Vector2 closest = (*pSightVec)[0];
	float ClosestDistanceSqr =
		Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pSightVec)[0]);
	
	for (size_t i = 0; i < pSightVec->size(); i++)
	{

		float CurrDistanceSqr = Elite::DistanceSquared(pAgent->GetAgentInfo().Position, (*pSightVec)[i]);
		if (CurrDistanceSqr < ClosestDistanceSqr)
		{
			closest = (*pSightVec)[i];
			ClosestDistanceSqr = CurrDistanceSqr;

		}
	}
	pBlackBoard->ChangeData("Target", closest);
	return true;
	
}

inline bool HasGun(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<ExpandedItemInfo>* ItemsInInventory = nullptr;
	pBlackBoard->GetData("ItemsInInventory", ItemsInInventory);

	if(!pAgent || !ItemsInInventory)
	{
	//	if (pAgent)
	//		pAgent->SetToFace(false, {});
		return false;
	}
	//pAgent->SetToFace(false, {});

	for(size_t i = 0; i < ItemsInInventory->size(); i++)
	{
		if ((*ItemsInInventory)[i].itemInfo.Type == eItemType::PISTOL)
			return true;
	}
	return false;
	
}

inline bool HasNoGun(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<ExpandedItemInfo>* ItemsInInventory = nullptr;
	pBlackBoard->GetData("ItemsInInventory", ItemsInInventory);

	if (!pAgent || !ItemsInInventory)
		return false;

	for (size_t i = 0; i < ItemsInInventory->size(); i++)
	{
		if ((*ItemsInInventory)[i].itemInfo.Type == eItemType::PISTOL)
			return false;
	}
	pAgent->SetToFace(false, {});
	return true;

}

inline bool WasBitten(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	float* time = nullptr;
	pBlackBoard->GetData("WasBittenCountDown", time);
	if(!time)
		return false;
	
	if(*time > 0.f)
	{
		Elite::Vector2 tv = pAgent->GetAgentInfo().Position - pAgent->GetAgentInfo().LinearVelocity * 5;
		if(pBlackBoard->ChangeData("FaceTarget",tv))
			return true;
			
		return false;
	}
	return false;
}

inline bool EnemyInGunSight(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<EnemyInfo>* pEnemys;
	pBlackBoard->GetData("EnemysInFOV", pEnemys);

	if (!pAgent || !pEnemys)
		return false;



	auto aPos = pAgent->GetAgentInfo().Position;
	auto aRot = pAgent->GetAgentInfo().Orientation;
	
	for (size_t i = 0; i < pEnemys->size();	i++)
	{
		//auto aEnemyDiffrence = (*pEnemys)[i].Location;
		Elite::Vector2 VecToEnemy = (*pEnemys)[i].Location - aPos;

		auto bRot = GetOrientationFromVelocity(VecToEnemy);

		float Result = abs(abs(aRot) - abs(bRot));
		if(Result < 0.01f)
			return true;
	}
	return false;
	
}

inline bool IsTargetFarAway(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target;
	//pBlackBoard->GetData("FaceTarget", target);

	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	if (!pAgent || !pActions)
	{
		return false;
	}

	pActions->CanRun = false;
	if (pAgent && pBlackBoard->GetData("Target", target))
	{
		float distance = Elite::Distance(pAgent->GetAgentInfo().Position, target);
		if (distance > 50.f)
			return true;
	}
	return false;
}

inline bool HasHalfStamina(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent{ nullptr };
	pBlackBoard->GetData("Agent", pAgent);
	Elite::Vector2 target;
	//pBlackBoard->GetData("FaceTarget", target);

	AgentActions* pActions{ nullptr };
	pBlackBoard->GetData("AgentActions", pActions);

	if (!pAgent || !pActions)
	{
		return false;
	}

	pActions->CanRun = false;
	if (pAgent && pBlackBoard->GetData("Target", target))
	{
		float distance = Elite::Distance(pAgent->GetAgentInfo().Position, target);
		if (distance > 20.f)
			return true;
	}
	return false;
}

inline bool ContineuAnyWays(Elite::Blackboard* pBLackBoard)
{
	return true;
}

inline bool MissingItemCloseBy(Elite::Blackboard* pBlackBoard)
{
	PlayerAgent* pAgent = nullptr;
	pBlackBoard->GetData("Agent", pAgent);
	std::vector<ExpandedItemInfo>* pItemVec = nullptr;
	pBlackBoard->GetData("FoundItems", pItemVec);

	std::vector<ExpandedItemInfo>* pItemInventory = nullptr;
	pBlackBoard->GetData("ItemsInInventory", pItemInventory);


	if (!pAgent || !pItemVec || pItemVec->size() < 1 || !pItemInventory )
		return false;

	if (pItemInventory->empty())
		return true;

	
	int gunsInInventory = 0;
	int foodInInventory = 0;
	int healsInINventory = 0;


	
	for (size_t i = 0; i < pItemInventory->size(); i++)
	{
		switch ((*pItemInventory)[i].itemInfo.Type)
		{
		case eItemType::PISTOL:
			gunsInInventory++;
			break;
		case eItemType::MEDKIT:
			healsInINventory++;
			break;
		case eItemType::FOOD:
			foodInInventory++;
			break;
		case eItemType::GARBAGE:
			break;
		case eItemType::RANDOM_DROP:
			break;
		case eItemType::RANDOM_DROP_WITH_CHANCE:
			break;
		default:
			break;
			;
		};
	}
	if (gunsInInventory + healsInINventory + foodInInventory == 5)
		return false;
	

	for(size_t i = 0; i < pItemVec->size(); i++)
	{
		
		if(Elite::Distance((*pItemVec)[i].itemInfo.Location,pAgent->GetAgentInfo().Position) < 500.f)
		{	
			bool shouldGrabItem = true;
			switch ((*pItemVec)[i].itemInfo.Type)
			{
			case eItemType::PISTOL:
				if (gunsInInventory >= 2)
					shouldGrabItem = false;
				break;
			case eItemType::MEDKIT:
				if (healsInINventory >= 2)
					shouldGrabItem = false;
				break;
			case eItemType::FOOD:
				if (foodInInventory >= 1)
					shouldGrabItem = false;
				break;
			default:
				shouldGrabItem = false;
				break;
			};
			if(shouldGrabItem)
			{
				pBlackBoard->ChangeData("Target", (*pItemVec)[i].itemInfo.Location);
				return true;
			}
		}
	}


	return false;
	
}