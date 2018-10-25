#include "base.h"
#include "world\physlistener.h"
#include "world\world.h"
#include "world\entities\entitybase.h"

CContactListener::CContactListener() {
}
CContactListener::~CContactListener() {
}

void CContactListener::BeginContact( b2Contact* pContact )
{
	PhysicsUserdata *pUserdataA, *pUserdataB;

	// Get the physics entities
	pUserdataA = static_cast<PhysicsUserdata*>(pContact->GetFixtureA()->GetBody()->GetUserData());
	pUserdataB = static_cast<PhysicsUserdata*>(pContact->GetFixtureB()->GetBody()->GetUserData());
	if( !pUserdataA || !pUserdataB ) {
		PrintError( L"Collision with missing entities\n" );
		return;
	}

	if( pUserdataA->pPhysicsObj )
		pUserdataA->pPhysicsObj->onPhysicsBeginContact( pContact->GetFixtureA(), pUserdataB->pPhysicsObj, pUserdataB->pRenderablePhysicsObj );
	else if( pUserdataA->pRenderablePhysicsObj )
		pUserdataA->pRenderablePhysicsObj->onPhysicsBeginContact( pContact->GetFixtureA(), pUserdataB->pPhysicsObj, pUserdataB->pRenderablePhysicsObj );

	if( pUserdataB->pPhysicsObj )
		pUserdataB->pPhysicsObj->onPhysicsBeginContact( pContact->GetFixtureB(), pUserdataA->pPhysicsObj, pUserdataA->pRenderablePhysicsObj );
	else if( pUserdataB->pRenderablePhysicsObj )
		pUserdataB->pRenderablePhysicsObj->onPhysicsBeginContact( pContact->GetFixtureB(), pUserdataA->pPhysicsObj, pUserdataA->pRenderablePhysicsObj );
}
void CContactListener::EndContact( b2Contact* pContact )
{
	PhysicsUserdata *pUserdataA, *pUserdataB;

	// Get the physics entities
	pUserdataA = static_cast<PhysicsUserdata*>(pContact->GetFixtureA()->GetBody()->GetUserData());
	pUserdataB = static_cast<PhysicsUserdata*>(pContact->GetFixtureB()->GetBody()->GetUserData());
	if( !pUserdataA || !pUserdataB ) {
		PrintError( L"Collision with missing entities\n" );
		return;
	}

	if( pUserdataA->pPhysicsObj )
		pUserdataA->pPhysicsObj->onPhysicsEndContact( pContact->GetFixtureA(), pUserdataB->pPhysicsObj, pUserdataB->pRenderablePhysicsObj );
	else if( pUserdataA->pRenderablePhysicsObj )
		pUserdataA->pRenderablePhysicsObj->onPhysicsEndContact( pContact->GetFixtureA(), pUserdataB->pPhysicsObj, pUserdataB->pRenderablePhysicsObj );

	if( pUserdataB->pPhysicsObj )
		pUserdataB->pPhysicsObj->onPhysicsEndContact( pContact->GetFixtureB(), pUserdataA->pPhysicsObj, pUserdataA->pRenderablePhysicsObj );
	else if( pUserdataB->pRenderablePhysicsObj )
		pUserdataB->pRenderablePhysicsObj->onPhysicsEndContact( pContact->GetFixtureB(), pUserdataA->pPhysicsObj, pUserdataA->pRenderablePhysicsObj );
}