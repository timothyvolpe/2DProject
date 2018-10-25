#pragma once

#include <Box2D\Box2D.h>
#include "entitybase.h"

#define PLAYER_SPRITE_SIZE 0.95f
#define PLAYER_JUMP_COOLDOWN 0.1;

class CTexture;

enum : int
{
	PLAYERFIXTUREID_UNKNOWN = 0,
	PLAYERFIXTUREID_BODY,
	PLAYERFIXTUREID_JUMPSENSOR
};

class CEntityPlayer : public CEntityRenderablePhysics
{
private:
	bool m_bControlledPlayer;

	unsigned char m_movementFlags;
	int m_iWalkableSurfacesContacting;

	double m_jumpCooldown;

	CTexture *m_pPlayerTexture;

	float m_fPlayerScale;
	float m_fPlayerMaxSpeed;

	b2Fixture *m_pPlayerFixture, *m_pPlayerJumpSensor;
	b2Body *m_pPlayerBody;

	void performMovement( double deltaT );
public:
	static float BaseMovementSpeed, BaseWalkSpeed, BaseRunSpeed; // meters per second

	CEntityPlayer();
	~CEntityPlayer();

	bool onCreate();
	void onDestroy();

	bool onActivate();

	void onDraw();

	void onUpdate( double deltaT );
	void onPhysTick( double deltaT );

	void onPhysicsBeginContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj );
	void onPhysicsEndContact( b2Fixture *pContactingFixture, CEntityPhysics *pPhysicsObj, CEntityRenderablePhysics *pRenderablePhysicsObj );

	bool isControlledPlayer() const;
	void setAsControlledPlayer( bool isControlledPlayer );
	float getPlayerScale() const;
	void setPlayerScale( float scale );
};