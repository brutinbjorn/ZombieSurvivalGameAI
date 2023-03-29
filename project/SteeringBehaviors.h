/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
using SteeringAgent = AgentInfo;
class Obstacle;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }
	void SetSecundairy(const  TargetData& target) { m_SecundiaryTarget = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
	TargetData m_SecundiaryTarget;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

/////////////////////////
//FLEE
//****
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	//Seek Behavior
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};


class Arrive : public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetSlowRadius(float NewSlowRadius) { m_SlowRadius = NewSlowRadius; }
	void SetTargetRadius(float NewTargetRadius) { m_ArrivalRadius = NewTargetRadius; }
private:
	float m_ArrivalRadius = 1.f;
	float m_SlowRadius = 15.f;

};

class Face : public Seek
{
public:
	Face() = default;
	virtual ~Face() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

};

class Wander : public ISteeringBehavior
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

private:
	float m_OffsetDistance = 6.f;
	float m_Radius = 4.f;
	float m_MaxAngleChange = Elite::ToRadians(45);
	float m_WanderAngle = 0.f;
};

class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

class Evade : public Pursuit
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetFleeRadius(float rad) { m_FleeRadius = rad; };
private:
	float m_FleeRadius = 20.f;
};
//
//class Hide : public Flee
//{
//public:
//	Hide(std::vector<Obstacle*>& obstaclesRef) :
//		nm_rObstacles(obstaclesRef) {};
//	virtual ~Hide() = default;
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//private:
//	const std::vector<Obstacle*>& nm_rObstacles;	
//};
//
//class Interpol : public Seek
//{
//public:
//	Interpol(std::vector<Obstacle*>& obstaclesRef) :
//		nm_rObstacles(obstaclesRef) {};
//	virtual ~Interpol() = default;
//	
//	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
//
//private:
//	const std::vector<Obstacle*>& nm_rObstacles;
// };

#endif



