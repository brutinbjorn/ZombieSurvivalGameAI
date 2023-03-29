//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
//#include "../SteeringAgent.h"
//#include "../Obstacle.h"
//#include "framework\EliteMath\EMatrix2x3.h"

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->MaxLinearSpeed;

	//debug Rendering
	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 1, 0, .5f),0.4f);
	
	return steering;
	
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering;

	steering = Seek::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity *= -1.f;

	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color(0, 0, 1, .5f), 0.4f);

	return steering;
}

SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{

	auto steering = Seek::CalculateSteering(deltaT, pAgent);
	
	const float maxSpeed = pAgent->MaxLinearSpeed;

	
	const float distance = (m_Target.Position - pAgent->Position).Magnitude();

	if (distance < m_ArrivalRadius)
	{
		steering.LinearVelocity = Elite::Vector2();
		return steering;
		
	}

	Elite::Vector2 velocity = steering.LinearVelocity;
	velocity.Normalize();
	if (distance < m_SlowRadius)
		steering.LinearVelocity = velocity * maxSpeed * distance / m_SlowRadius;
	
	return steering;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{

	//pAgent->(false);
	SteeringOutput steering{};
	steering.AutoOrient = false;
	
	Elite::Vector2 TargetVector = { (m_Target).Position - pAgent->Position };
	TargetVector.Normalize();
	float angle = GetOrientationFromVelocity(TargetVector) - pAgent->Orientation;
	angle = Elite::ToDegrees(angle);
	//angle += 90.f;
	if (angle > 180.f) 
	{
		angle = angle - 360.f;
	}
	if (angle < -180.f)
	{
		angle = angle + 360.f;
	}

	if (angle > 1.f || angle < -1.f)
	{
		std::cout << angle << std::endl;
	}

	steering.AngularVelocity = angle;

	//DEBUG RENDERING
	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	return steering;
	
}

SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	using namespace Elite;
	SteeringOutput steering = {};
	float Angle = (m_MaxAngleChange / 2) - (rand()) / ((RAND_MAX / m_MaxAngleChange));
	
	m_WanderAngle += Angle;
	float amplitude = pAgent->Orientation + ToRadians(90.f) + m_WanderAngle;
	Elite::Vector2 wanderpoint = OrientationToVector(amplitude) * m_Radius;

	steering.LinearVelocity = pAgent->LinearVelocity.GetNormalized() * m_OffsetDistance + wanderpoint;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->MaxLinearSpeed;

	//if (pAgent->CanRenderBehavior())
	//{
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	//	DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition() + pAgent->GetDirection() * m_OffsetDistance, m_Radius, { 0,0,1 }, 0.4f);
	//	DEBUGRENDERER2D->DrawPoint(pAgent->GetPosition() + pAgent->GetDirection() * m_OffsetDistance + wanderpoint, 3, { 1,0,0 }, 0.4f);
	//}
	
	
	return steering;
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	float distanceToTarget = (m_Target.Position - pAgent->Position).Magnitude();
	float TimeToTravelDistance = distanceToTarget / pAgent->MaxLinearSpeed;

	Elite::Vector2 InterseptPoint = m_Target.Position;
	InterseptPoint += m_Target.LinearVelocity * TimeToTravelDistance;
	
	steering.LinearVelocity = InterseptPoint - pAgent->Position;
	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
	steering.LinearVelocity *= pAgent->MaxLinearSpeed; //Rescale to Max Speed

	//DEBUG RENDERING
	//if (pAgent->CanRenderBehavior())
	//{
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
	//	DEBUGRENDERER2D->DrawPoint(InterseptPoint, 3.f, Elite::Color(1,0,0,0.5f), 0.4f);
	//}
		

	return steering;
	
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	if ((m_Target.Position - pAgent->Position).Magnitude() > m_FleeRadius)
	{
		steering.IsValid = false;
		return steering;
		
	}
	steering = Pursuit::CalculateSteering(deltaT, pAgent);
	steering.LinearVelocity *= -1.f;

	//DEBUG RENDERING
	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);

	return steering;

}
//
//SteeringOutput Hide::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering = {};
//
//	if (nm_rObstacles.size() <= 0)
//	{
//		return steering = Flee::CalculateSteering(deltaT, pAgent);
//	}
//	const Obstacle* closest = nm_rObstacles[0];
//	
//	for (unsigned int i = 0; i < nm_rObstacles.size(); i++)
//	{
//		const float currentSmallestDist = closest->GetCenter().Distance(pAgent->GetPosition());
//		if(nm_rObstacles[i]->GetCenter().Distance(pAgent->GetPosition()) < currentSmallestDist)
//			closest = nm_rObstacles[i];
//	}
//
//	Elite::Vector2 TargetLookAt = closest->GetCenter() - m_Target.Position;
//	TargetLookAt.Normalize();
//	TargetLookAt *= pAgent->GetRadius() * 2 + closest->GetRadius();
//
//	Elite::Vector2 hidingPosition = TargetLookAt + closest->GetCenter();
//
//	steering.LinearVelocity = hidingPosition - pAgent->GetPosition();
//	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
//	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed
//	if (pAgent->CanRenderBehavior())
//	{
//		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
//		DEBUGRENDERER2D->DrawPoint(hidingPosition, 3.f, Elite::Color(0, 0, 1, 0.5f), 0.4f);
//	}
//
//	return steering;
//}
//
//SteeringOutput Interpol::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//
//	SteeringOutput steering = {};
//
//	if (nm_rObstacles.size() <= 0)
//	{
//		return steering = Seek::CalculateSteering(deltaT, pAgent);
//	}
//	const Obstacle* closest = nm_rObstacles[0];
//
//	for (unsigned int i = 0; i < nm_rObstacles.size(); i++)
//	{
//		const float currentSmallestDist = closest->GetCenter().Distance(pAgent->GetPosition());
//		if (nm_rObstacles[i]->GetCenter().Distance(pAgent->GetPosition()) < currentSmallestDist)
//			closest = nm_rObstacles[i];
//	}
//
//	Elite::Vector2 CenterPoint = (closest->GetCenter() + m_Target.Position) / 2;
//	steering.LinearVelocity = CenterPoint - pAgent->GetPosition();
//	steering.LinearVelocity.Normalize(); //Normalize Desired Velocity
//	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed(); //Rescale to Max Speed
//	if (pAgent->CanRenderBehavior())
//	{
//		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0.4f);
//		DEBUGRENDERER2D->DrawPoint(CenterPoint, 3.f, Elite::Color(0, 1, 0, 0.5f), 0.4f);
//	}
//
//	return steering;
//}


 
