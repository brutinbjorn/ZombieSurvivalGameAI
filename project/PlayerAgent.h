#pragma once
#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


#include "EDecisionMaking.h"
#include "SteeringBehaviors.h"

class PlayerAgent
{
public:
	PlayerAgent(/*IExamInterface* InterfaceRef*/);
	~PlayerAgent();

	void Update(float dt) ;
	void UpdateSteering(IExamInterface* interf,float dt);

	void SetAgentInfo(AgentInfo info);
	void LinkInterFace(IExamInterface* interf);
	AgentInfo GetAgentInfo() const;

	void SetDicisionMaking(Elite::IDecisionMaking* decisionStructure);

	SteeringOutput GetSteeringOutput() const { return m_SteeringOutput; };
	 
	//set Steering
	void SetToWander() { SetSteeringBehavior(m_pWanderBehavior); }
	void SetToSeek(Elite::Vector2 SeekPos)
	{
		m_Target = SeekPos;
	//	Elite::Vector2 navPoint = m_pExamInterface->NavMesh_GetClosestPathPoint(m_Target);
		m_pSeekBehavior->SetTarget(SeekPos);
		SetSteeringBehavior(m_pSeekBehavior);
	}
	void SetToFlee(Elite::Vector2 fleeTarget)
	{
		//m_Target = fleeTarget;
		m_pFleeBehavior->SetTarget(fleeTarget);
		SetSteeringBehavior(m_pFleeBehavior);
	}
	void SetToFace(bool FaceSometing,Elite::Vector2 FaceTarget)
	{
		m_FaceSomething = FaceSometing;
		m_pFaceBehavior->SetTarget(FaceTarget);
	}

	
	bool GetPickupItem() { return m_PickupItem; };
	void SetPickUpItem(bool pickStuffUp) { m_PickupItem = pickStuffUp;} ;

private:
	void SetSteeringBehavior(ISteeringBehavior* newSteering) { m_pActiveSteeringBehavior = newSteering; }

	Elite::IDecisionMaking* m_pDecisionMaking = nullptr;
	AgentInfo m_AgentInfo;

	//Vals
	SteeringOutput m_SteeringOutput;
	IExamInterface* m_pExamInterface;
	Elite::Vector2 m_Target;
	bool m_PickupItem = false;
	
	// Behaviors
	ISteeringBehavior* m_pActiveSteeringBehavior = nullptr;


	bool m_FaceSomething = false;
	Wander* m_pWanderBehavior = nullptr;
	Evade* m_pEvadeBehavior = nullptr;
	Arrive* m_pArriveBehavior = nullptr;
	Seek* m_pSeekBehavior = nullptr;
	Flee* m_pFleeBehavior = nullptr;
	Face* m_pFaceBehavior = nullptr;
	
	
};
