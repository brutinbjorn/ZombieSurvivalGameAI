#include "stdafx.h"
#include "PlayerAgent.h"

PlayerAgent::PlayerAgent()
{
//	m_pExamInterface = interface;
	//Behaviors
	m_AgentInfo = {};
	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();
	m_pArriveBehavior = new Arrive();
	m_pSeekBehavior = new Seek();
	m_pFleeBehavior = new Flee();
	m_pFaceBehavior = new Face();

	m_pActiveSteeringBehavior = m_pWanderBehavior;
}

PlayerAgent::~PlayerAgent()
{
	//Behaviors
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pArriveBehavior);
	SAFE_DELETE(m_pEvadeBehavior);
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pFleeBehavior);

	SAFE_DELETE(m_pDecisionMaking);
}


//updates The DecisionMAking
 void PlayerAgent::Update(float dt) 
{
	if (m_pDecisionMaking)
		m_pDecisionMaking->Update(dt);

	
}

void PlayerAgent::UpdateSteering(IExamInterface* interf,float dt)
{
	m_pExamInterface = interf;
	Elite::Vector2 el= m_pExamInterface->NavMesh_GetClosestPathPoint(m_Target);

	m_pActiveSteeringBehavior->SetTarget(el);
	
	if (m_pActiveSteeringBehavior)
		m_SteeringOutput = m_pActiveSteeringBehavior->CalculateSteering(dt, &m_AgentInfo);

	m_SteeringOutput.AutoOrient = !m_FaceSomething;
	if (!m_SteeringOutput.AutoOrient)
	{
		auto faceVal = m_pFaceBehavior->CalculateSteering(dt, &m_AgentInfo);
		m_SteeringOutput.AngularVelocity = faceVal.AngularVelocity;
		
	}

	
}


void PlayerAgent::SetAgentInfo(AgentInfo info)
{
	m_AgentInfo = info;
}

void PlayerAgent::LinkInterFace(IExamInterface* interf)
{
	m_pExamInterface = interf;
}

AgentInfo PlayerAgent::GetAgentInfo() const
{
	return m_AgentInfo;
}

void PlayerAgent::SetDicisionMaking(Elite::IDecisionMaking* decisionStructure)
{
	m_pDecisionMaking = decisionStructure;
}
