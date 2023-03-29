#pragma once
#include "EBlackboard.h"
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "PlayerAgent.h"
#include "SteeringBehaviors.h"
#include "StructsInfoOverloads.h"


class IBaseInterface;
class IExamInterface;

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	vector<HouseInfo> GetHousesInFOV() const;
	vector<EntityInfo> GetEntitiesInFOV() const;

	Elite::Blackboard* CreateBlackBoard();
	
	vector<ItemInfo> m_ItemsInFOV;
	vector<EnemyInfo> m_EnemysInFOV;
	vector<PurgeZoneInfo> m_PurgeZonesInPOV;
	//vector<ItemInfo> m_ExtraFoundItems;

	vector<ExpandedItemInfo> m_ItemsInventory;
	
	vector<HouseInfoExpanded> m_FoundHouses;
	vector<ExpandedItemInfo> m_FoundItems;
	
	Elite::Vector2 m_Target = {};

	AgentActions* m_pActions = nullptr;
	bool m_GotHit = false;
	bool m_CanRun = false; //Demo purpose
	bool m_pGrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	float m_EatDelay = 0.5f;
	float m_HealDelay = 0.5f;
	
	float m_RespondToBitten = 0.f;
	
	vector<Elite::Vector2> m_pFarSightPoints;
	
	/// playerAgent = contains info and behavior
	PlayerAgent* m_pPlayerAgent = nullptr;
	
	/// <Behaviors>
	//Wander* m_pWanderBehavior = nullptr;
	//Evade* m_pEvadeBehavior = nullptr;
	//Arrive* m_pArriveBehavior = nullptr;
	//Seek* m_pSeekBehavior = nullptr;
	//Flee* m_pFleeBehavior = nullptr;
	
	
	
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}