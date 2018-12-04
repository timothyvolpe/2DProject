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

	assert( pUserdataA->safety == 12345 && pUserdataB->safety == 12345 );

	if( pUserdataA->type == USER_DATA_TYPE_ENTITIES ) {
		if( pUserdataA->pPhysicsObj )
			pUserdataA->pPhysicsObj->onPhysicsBeginContact( pContact->GetFixtureA(), pUserdataB );
		else if( pUserdataA->pRenderablePhysicsObj )
			pUserdataA->pRenderablePhysicsObj->onPhysicsBeginContact( pContact->GetFixtureA(), pUserdataB );
	}

	if( pUserdataB->type == USER_DATA_TYPE_ENTITIES ) {
		if( pUserdataB->pPhysicsObj )
			pUserdataB->pPhysicsObj->onPhysicsBeginContact( pContact->GetFixtureB(), pUserdataA );
		else if( pUserdataB->pRenderablePhysicsObj )
			pUserdataB->pRenderablePhysicsObj->onPhysicsBeginContact( pContact->GetFixtureB(), pUserdataA );
	}
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

	assert( pUserdataA->safety == 12345 && pUserdataB->safety == 12345 );

	if( pUserdataA->type == USER_DATA_TYPE_ENTITIES ) {
		if( pUserdataA->pPhysicsObj )
			pUserdataA->pPhysicsObj->onPhysicsEndContact( pContact->GetFixtureA(), pUserdataB );
		else if( pUserdataA->pRenderablePhysicsObj )
			pUserdataA->pRenderablePhysicsObj->onPhysicsEndContact( pContact->GetFixtureA(), pUserdataB );
	}
	
	if( pUserdataB->type == USER_DATA_TYPE_ENTITIES ) {
		if( pUserdataB->pPhysicsObj )
			pUserdataB->pPhysicsObj->onPhysicsEndContact( pContact->GetFixtureB(), pUserdataA );
		else if( pUserdataB->pRenderablePhysicsObj )
			pUserdataB->pRenderablePhysicsObj->onPhysicsEndContact( pContact->GetFixtureB(), pUserdataA );
	}
}