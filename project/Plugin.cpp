#include "stdafx.h"
#include "Plugin.h"

#include "Behaviors.h"

#include "IExamInterface.h"
#include "PlayerAgent.h"


//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Eddie Wally";
	info.Student_FirstName = "Bjorn";
	info.Student_LastName = "Brutin";
	info.Student_Class = "2DAE14";

}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded

	// Agent
	m_pPlayerAgent = new PlayerAgent(/*m_pInterface*/);
	m_pActions = new AgentActions();
	//m_ItemsInVentory.resize(m_pInterface->Inventory_GetCapacity()) ;
	
	// Behaviors;
	//m_pWanderBehavior = new Wander();
	//m_pArriveBehavior = new Arrive();
	//m_pEvadeBehavior = new Evade();
	//m_pFleeBehavior = new Flee();
	//m_pSeekBehavior = new Seek();

	Elite::Blackboard* pBlackboard = CreateBlackBoard();
	
	Elite::BehaviorTree* pBehaviorTree = new Elite::BehaviorTree(pBlackboard,
		new Elite::BehaviorSelector
		({
			//root
			//run Both?
		new Elite::BehaviorSequence({
				//only 1 should ALWAYS Run,
			new Elite::BehaviorSelector({
				
				//flee from KillZone;
				new Elite::BehaviorSequence({
					new Elite::BehaviorConditional(NewPurgeZoneInFOV),
					new Elite::BehaviorAction(ChangeToFlee)
				}),

				new Elite::BehaviorSequence({//look for item
					new Elite::BehaviorConditional(NewItemInFOV),
					new Elite::BehaviorConditional(ShouldGrabItem),
					new Elite::BehaviorAction(SeekItem),
					new Elite::BehaviorAction(GrabItem)
				}),
				
				new Elite::BehaviorSequence({//enter house
					new Elite::BehaviorConditional(NewHouseInFOV),
					new Elite::BehaviorAction(ChangeToSeek)
				}),

				new Elite::BehaviorSequence({//RememberItem
					new Elite::BehaviorConditional(MissingItemCloseBy),
					new Elite::BehaviorAction(ChangeToSeek)
				}),
				
				new Elite::BehaviorSequence({ // look for intersing point in the distance
					new Elite::BehaviorConditional(NewPointOnFarSight),
					new Elite::BehaviorAction(ChangeToSeek)
				}),
				//muck about
				new Elite::BehaviorAction(ChangeToWander)
			}),
			

			new Elite::BehaviorSelector({
				new Elite::BehaviorSequence({// fire Gun At Enemys
					new Elite::BehaviorConditional(HasGun),
					new Elite::BehaviorAction(FaceEnemy),
					new Elite::BehaviorConditional(EnemyInGunSight),
					new Elite::BehaviorAction(UseGun)
				}),
					
				new Elite::BehaviorSequence({// panic when hit(turn around and look at enemy)
					new Elite::BehaviorConditional(HasGun),
					new Elite::BehaviorConditional(WasBitten),
					new Elite::BehaviorAction(LookBackwards)
				}),
				
				new Elite::BehaviorSequence({
					new Elite::BehaviorConditional(HasNoGun),
					new Elite::BehaviorAction(ShouldRun)
				})
			})
			//
			//// these should Run in Addition to the top ones// shooting logic
			//new Elite::BehaviorSequence({
			//	new Elite::BehaviorConditional(IsTargetFarAway),
			//	new Elite::BehaviorAction(StartRunning)
			//}),
			
		})
	}));

	m_pPlayerAgent->SetDicisionMaking(pBehaviorTree);
}

//Called only once
void Plugin::DllShutdown()
{
	//Called when the plugin gets unloaded
	SAFE_DELETE(m_pPlayerAgent);
		
}


Elite::Blackboard* Plugin::CreateBlackBoard()
{
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	
	pBlackboard->AddData("Agent", m_pPlayerAgent);
	pBlackboard->AddData("HouseVec", &m_FoundHouses);
	//pBlackboard->AddData("WorldSize", m_TrimWorldSize);
	pBlackboard->AddData("Target", Elite::Vector2{});
	pBlackboard->AddData("FaceTarget", Elite::Vector2{});
	pBlackboard->AddData("ItemTarget", static_cast<ItemInfo*>(nullptr));

	pBlackboard->AddData("ItemsInInventory", &m_ItemsInventory);
	pBlackboard->AddData("FoundItems", &m_FoundItems);
	pBlackboard->AddData("EnemysInFOV", &m_EnemysInFOV);
	pBlackboard->AddData("ItemsInFOV", &m_ItemsInFOV);
	pBlackboard->AddData("PurgeZonesInFOV", &m_PurgeZonesInPOV);

	pBlackboard->AddData("AgentActions", m_pActions);
	pBlackboard->AddData("FarSightVec", &m_pFarSightPoints);

	pBlackboard->AddData("WasBittenCountDown", &m_RespondToBitten);
	
	pBlackboard->AddData("Time", 0.0f);

	return pBlackboard;
	
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_pGrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{



#pragma region PREUPDATE
	auto steering = SteeringPlugin_Output();

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework


	m_pActions->Shoot = false;
	m_pActions->ShouldRun = false;

	auto agentInfo = m_pInterface->Agent_GetInfo();
	
	if (m_RespondToBitten >= 0.f)
		m_RespondToBitten -= dt;
	
	if (agentInfo.Bitten)
	{
		m_RespondToBitten = 2.f;
	}


	
	
	auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)


	m_ItemsInventory.clear();
	for (size_t i = 0; i < m_pInterface->Inventory_GetCapacity(); i++)
	{
		ItemInfo a;
		bool gotitem = m_pInterface->Inventory_GetItem(i,a);
		if (gotitem)
		{
			ExpandedItemInfo ToAdd;
			ToAdd.itemInfo = a;
			switch (a.Type)
			{
			case eItemType::PISTOL:
				ToAdd.uses = m_pInterface->Weapon_GetAmmo(a);
				break;
			case eItemType::MEDKIT:
				ToAdd.uses = m_pInterface->Medkit_GetHealth(a);
				break;
			case eItemType::FOOD:
				ToAdd.uses = m_pInterface->Food_GetEnergy(a);
				break;
			default: 
				ToAdd.uses = -1;
				break;
			}
			m_ItemsInventory.push_back(ToAdd);
		}
		else
		{
			a = ItemInfo{ eItemType::RANDOM_DROP,{},0 };
			ExpandedItemInfo ToAdd;
			ToAdd.itemInfo = a;
			ToAdd.uses = -1;
			m_ItemsInventory.push_back(ToAdd);
		}
	}
		


	for (auto& h : vHousesInFOV)
	{
		bool houseInList = false;
		for(size_t i = 0; i < m_FoundHouses.size(); i++)
		{
			if (m_FoundHouses[i].Center == h.Center) // check if house in FOV is already in the list
			{
				houseInList = true;
				break;
			}
		}
		if (!houseInList) // if not in the list add to list of found houses
			m_FoundHouses.push_back(h);
	}

	
	for(auto& h : m_FoundHouses)
	{
		if(Elite::Distance(h.Center,agentInfo.Position) <= 2.f)
		{
			h.CenterVisited = true;
			// check sides
			//	auto checkleft = h.Center;
			//	checkleft.x -= (h.Size.x + 5);
			//auto leftpoint = m_pInterface->NavMesh_GetClosestPathPoint(checkleft);
			//if(leftpoint.x > h.Center.x 
			//	&& leftpoint.y < h.Center.y + h.Size.y 
			//	&& leftpoint.y > h.Center.y = h.Size.y)
			//{
			//	h.LeftDoor = true;
			//}
			
			
		}
		if (h.CenterVisited)
			h.lastTimeVisited += dt;
		
	}



	
	m_ItemsInFOV.clear();
	m_EnemysInFOV.clear();
	m_PurgeZonesInPOV.clear();
	for (auto& e : vEntitiesInFOV)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			m_pInterface->PurgeZone_GetInfo(e, zoneInfo);
			m_PurgeZonesInPOV.push_back(zoneInfo);
			std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y <<  " ---EntityHash: " << e.EntityHash << "---Radius: "<< zoneInfo.Radius << std::endl;
		}

		if(e.Type == eEntityType::ENEMY)
		{
			EnemyInfo Enemy;
			m_pInterface->Enemy_GetInfo(e,Enemy);
			m_EnemysInFOV.push_back(Enemy);
			std::cout << "Enemy in FOV" << e.Location.x << "" << e.Location.y << std::endl;
		}


		if(e.Type == eEntityType::ITEM)
		{
			ItemInfo item;
			m_pInterface->Item_GetInfo(e, item);
			m_ItemsInFOV.push_back(item);

			ExpandedItemInfo newItem{ item };
			//newItem.uses = item.Type;

			bool itemAlreadyInList = false;
			for(size_t i = 0; i <m_FoundItems.size();i++)
			{
				if(m_FoundItems[i].itemInfo.Location == item.Location)
				{
					itemAlreadyInList = true;
 					break;
				}
			}
			if(!itemAlreadyInList)
			m_FoundItems.push_back(newItem);
			
			std::cout << "item in FOV" << e.Location.x << "" << e.Location.y << std::endl;

		}
	}
	

	//farsight, look for points in the navmesh that are not a striaght line, IE something is there but no idea what;
	// cuz not seeing a house thats farther then 10 m is basicly being blind, zombies see farter tyhen that;
	auto worldcenter = m_pInterface->World_GetInfo().Center;
	auto worldsize = m_pInterface->World_GetInfo().Dimensions;
	auto worldSizeSplit = worldsize / 20;
	

	if(m_pFarSightPoints.empty())
	{		
		for (int worldheight = -10; worldheight < 10; worldheight++)
		{
			for (int worldwidth = -10; worldwidth < 10; worldwidth++)
			{
				Elite::Vector2 StraightToPoint = worldcenter;
				StraightToPoint += 
					Elite::Vector2(worldSizeSplit.x * worldwidth,worldSizeSplit.y * worldheight);
				Elite::Vector2 result = m_pInterface->NavMesh_GetClosestPathPoint(StraightToPoint);

				if(result != StraightToPoint)
				{
					m_pFarSightPoints.push_back(result);
				}
			}
		}
	}

	if(m_pFarSightPoints.size() > 0)
	{
		for (size_t i = 0; i < m_pFarSightPoints.size(); ++i)
		{
			if(agentInfo.Position.Distance(m_pFarSightPoints[i]) <= 10.f)
			{
				auto val= m_pFarSightPoints[i];
				m_pFarSightPoints[i] = m_pFarSightPoints.back();
				m_pFarSightPoints.back() = val;
				m_pFarSightPoints.pop_back();
			}
		}
	}

	
	
#pragma endregion PREUPDATE
	// m_pInterface->NavMesh_GetClosestPathPoint();


	//Simple Seek Behaviour (towards Target)
	//steering.LinearVelocity = nextTargetPos - agentInfo.Position; //Desired Velocity
	//steering.LinearVelocity = Elite::Vector2(10.f, 0.f);
	//steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	//steering.LinearVelocity *= agentInfo.MaxLinearSpeed; //Rescale to Max Speed



	m_pPlayerAgent->SetAgentInfo(agentInfo);
	
	m_pPlayerAgent->Update(dt);
	m_pPlayerAgent->UpdateSteering(m_pInterface,dt);

	//m_pInterface->NavMesh_GetClosestPathPoint();
	
	auto otherSteering = m_pPlayerAgent->GetSteeringOutput();//m_pWanderBehavior->CalculateSteering(dt, &agentInfo);

	//INVENTORY USAGE
	//********************

	m_pGrabItem = m_pActions->GrabItem;
	
	if (m_pGrabItem)
	{
		ItemInfo item;
		//Item_Grab > When DebugParams.AutoGrabClosestItem is TRUE, the Item_Grab function returns the closest item in range
		//Keep in mind that DebugParams are only used for debugging purposes, by default this flag is FALSE
		//Otherwise, use GetEntitiesInFOV() to retrieve a vector of all entities in the FOV (EntityInfo)
		//Item_Grab gives you the ItemInfo back, based on the passed EntityHash (retrieved by GetEntitiesInFOV)
		int gunsInInventory = 0;
		int foodInInventory = 0;
		int healsInINventory = 0;

		for (size_t i = 0; i < m_ItemsInventory.size(); i++)
		{
			switch ((m_ItemsInventory)[i].itemInfo.Type)
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

		for(uint8_t i = 0; i < vEntitiesInFOV.size(); i++)
		{
			if(vEntitiesInFOV[i].Type == eEntityType::ITEM)
			{
				ItemInfo a;
				m_pInterface->Item_GetInfo(vEntitiesInFOV[i], a);
				
				bool shouldGrabItem = true;
				switch (a.Type)
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
					break;
				};

				if (m_pInterface->Item_Grab(vEntitiesInFOV[i], item) 
					&& shouldGrabItem)
				{
					//Once grabbed, you can add it to a specific inventory slot
					//Slot must be empty
					ItemInfo info;
					// look for a slot that is empty
					for (uint8_t i = 0; i < m_pInterface->Inventory_GetCapacity(); i++)
					{
						if (!m_pInterface->Inventory_GetItem(i, info ))
						{
							m_pInterface->Inventory_AddItem(i, item);
							for (size_t j = 0; j < m_FoundItems.size(); ++j)
							{
								if (m_FoundItems[j].itemInfo.Location == item.Location)
								{
									std::swap(m_FoundItems[j], m_FoundItems.back());
									m_FoundItems.pop_back();
								}
								
							}
						}
					}
				}				
			}
			
		}

	}




	bool ShouldShoot = m_pActions->Shoot;
	bool ShouldEat = (m_pPlayerAgent->GetAgentInfo().Energy < 5.f);
	bool ShouldHeal = (m_pPlayerAgent->GetAgentInfo().Health < 5.f);
	
	if (ShouldShoot || ShouldEat || ShouldHeal)
	{
		m_pActions->UseItem = false;
		
		
		if(ShouldShoot && m_pActions->ShootDelay <= 0.f)
		{

			m_pActions->Shoot = false;
			
			int gunInSlot = -1;
			int leastbullets = 0;
			for (size_t i = 0; i < m_ItemsInventory.size(); ++i)
			{
				
				if (m_ItemsInventory[i].itemInfo.Type == eItemType::PISTOL)
				{
					if (gunInSlot == -1)
						gunInSlot = i;
					else
					{
						if (m_ItemsInventory[gunInSlot].uses > m_ItemsInventory[i].uses)
							gunInSlot = i;
					}

				}
			}
			if (gunInSlot != -1)
			{
				m_pActions->ShootDelay += 0.5f;
				m_pActions->ShotToFire--;
				m_pInterface->Inventory_UseItem(gunInSlot);
			}
		}
		else if(ShouldEat && m_EatDelay <= 0.f)
		{
			int FoodInSlot = -1;
			for (size_t i = 0; i < m_ItemsInventory.size(); ++i)
			{
				if (m_ItemsInventory[i].itemInfo.Type == eItemType::FOOD)
				{
					FoodInSlot = i;
					break;
				}
			}
			if (FoodInSlot != -1)
			{
				m_EatDelay += 0.5f;
				m_pInterface->Inventory_UseItem(FoodInSlot);
				
			}
		}
		else if(ShouldHeal && m_HealDelay <= 0.f)
		{
			int HealsInSlot = -1;
			for (size_t i = 0; i < m_ItemsInventory.size(); ++i)
			{
				if (m_ItemsInventory[i].itemInfo.Type == eItemType::MEDKIT)
				{
					HealsInSlot = i;
					break;
				}
			}
			if (HealsInSlot != -1)
			{
				m_HealDelay += 0.5f;
				m_pInterface->Inventory_UseItem(HealsInSlot);
				
			}
		}
	}

	if (m_pActions->ShootDelay > 0.f)
		m_pActions->ShootDelay -= dt;

	if (m_HealDelay > 0.f)
		m_HealDelay -= dt;

	if (m_EatDelay > 0.f)
		m_EatDelay -= dt;
	
	m_RemoveItem = m_pActions->RemoveItem;
	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		//m_pInterface->Inventory_RemoveItem(0);
	}

	ItemInfo autoDrop;
	for (uint8_t i = 0; i < m_ItemsInventory.size(); i++)
	{
		if(m_ItemsInventory[i].uses == 0)
			m_pInterface->Inventory_RemoveItem(i);
		
		m_pInterface->Inventory_GetItem(i, autoDrop);

		if(m_ItemsInventory[i].itemInfo.Type == eItemType::GARBAGE)
			m_pInterface->Inventory_RemoveItem(i);
	}


	//otherSteering->cal

	steering.LinearVelocity = otherSteering.LinearVelocity;
	steering.AngularVelocity = otherSteering.AngularVelocity;
	//IsValid = other.IsValid;

	steering.AutoOrient = otherSteering.AutoOrient;
	//steering.RunMode = otherSteering.RunMode;
	steering.RunMode = m_pActions->CanRun;

	//steering.AngularVelocity = m_AngSpeed += 1.f; //Rotate your character to inspect the world while walking
	//steering.AutoOrient = false; //Setting AutoOrientate to TRue overrides the AngularVelocity

	//steering.RunMode = m_CanRun; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

								 //SteeringPlugin_Output is works the exact same way a SteeringBehaviour output
								 //@End (Demo Purposes)

	//m_pGrabItem ; //Reset State
	//m_UseItem  ;
	//m_RemoveItem ;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });

	for (size_t i = 0; i < m_EnemysInFOV.size(); ++i)
	{
		auto val = m_EnemysInFOV[i];
		m_pInterface->Draw_Circle(val.Location, val.Size + 3,Elite::Vector3(255,0,0));
	}

	for (size_t i = 0; i < m_ItemsInFOV.size(); ++i)
	{
		auto val = m_ItemsInFOV[i];
		m_pInterface->Draw_Circle(val.Location,  5, Elite::Vector3(255, 0, 0));

	}

	for (size_t i = 0; i < m_FoundItems.size(); i++)
	{
		auto val = m_FoundItems[i];
		m_pInterface->Draw_Circle(val.itemInfo.Location, 2, Elite::Vector3(255, 0, 255));
	}
	
	for(size_t i = 0;i < m_pFarSightPoints.size(); ++i)
	{
		m_pInterface->Draw_Circle(m_pFarSightPoints[i], 1, { 125,125,0 });
	}


	for(size_t i = 0; i < m_FoundHouses.size(); ++i)
	{
		auto house = m_FoundHouses[i];
		std::vector<Elite::Vector2> borders;

		Elite::Vector2 halfSize = m_FoundHouses[i].Size/2 + Elite::Vector2(0.5,0.5);
		
		borders.push_back(m_FoundHouses[i].Center - halfSize);
		borders.push_back(m_FoundHouses[i].Center + Elite::Vector2{-halfSize.x, halfSize.y});
		borders.push_back(m_FoundHouses[i].Center + Elite::Vector2{ halfSize.x, halfSize.y });
		borders.push_back(m_FoundHouses[i].Center + Elite::Vector2{ halfSize.x, -halfSize.y });

		m_pInterface->Draw_Polygon(&borders[0],4,Elite::Vector3{0,255,0});

		//draw openings

		m_pInterface->Draw_Point(m_FoundHouses[i].Center + Elite::Vector2{ -halfSize.x - 1,0 }, 4, 
			Elite::Vector3{ float(255 * !m_FoundHouses[i].LeftDoor),float(255 * m_FoundHouses[i].LeftDoor),0 });

		m_pInterface->Draw_Point(m_FoundHouses[i].Center + Elite::Vector2{ halfSize.x + 1,0 }, 4,
			Elite::Vector3{ float(255 * !m_FoundHouses[i].RightDoor),float(255 * m_FoundHouses[i].RightDoor),0 });
		
		m_pInterface->Draw_Point(m_FoundHouses[i].Center + Elite::Vector2{ 0,halfSize.y+1 }, 4,
			Elite::Vector3{ float(255 * !m_FoundHouses[i].TopDoor),float(255 * m_FoundHouses[i].TopDoor),0 });

		m_pInterface->Draw_Point(m_FoundHouses[i].Center + Elite::Vector2{ 0,-halfSize.y - 1 }, 4,
			Elite::Vector3{ float(255 * !m_FoundHouses[i].BottemDoor),float(255 * m_FoundHouses[i].BottemDoor),0 });
		
	}
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}
