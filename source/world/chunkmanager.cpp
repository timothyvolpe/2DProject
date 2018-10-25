#include "base.h"
#include "world\chunkmanager.h"
#include "world\chunk.h"

glm::ivec2 CChunkManager::RelativeToAbsolutePos( glm::ivec2 origin, CChunk *pChunk ) {
	return glm::ivec2( pChunk->getRelativePosition().x + origin.x, pChunk->getRelativePosition().y + origin.y );
}

CChunkManager::CChunkManager() {
	m_chunksRenderedX = 3;
	m_chunksRenderedY = 3;
	m_chunksOrigin = glm::ivec2( 0, 0 );
}
CChunkManager::~CChunkManager() {
}

bool CChunkManager::initialize()
{
	if( !this->allocateChunks() )
		return false;

	return true;
}
void CChunkManager::destroy()
{
	this->destroyChunks();
}

bool CChunkManager::allocateChunks()
{
	int chunkCount;

	chunkCount = m_chunksRenderedX * m_chunksRenderedY;
	if( chunkCount <= 0 ) {
		PrintError( L"No chunks will be rendered\n" );
		return false;
	}

	// If chunks already exist, destroy them
	if( m_chunksRendered.size() > 0 )
		this->destroyChunks();

	// Allocate new chunks
	m_chunksRendered.resize( m_chunksRenderedX );
	for( int x = 0; x < m_chunksRenderedX; x++ )
	{
		m_chunksRendered[x].reserve( m_chunksRenderedY );
		for( int y = 0; y < m_chunksRenderedY; y++ ) {
			CChunk *pNewChunk;
			pNewChunk = new CChunk();
			if( !pNewChunk->initialize() )
				return false;
			pNewChunk->setRelativePosition( glm::ivec2( x, y ) );
			m_chunksRendered[x].push_back( pNewChunk );
		}
	}
	// Realign them
	this->setChunksOrigin( this->getChunksOrigin() );

	return true;
}
void CChunkManager::destroyChunks()
{
	for( unsigned int x = 0; x < m_chunksRendered.size(); x++ ) {
		for( unsigned int y = 0; y < m_chunksRendered[x].size(); y++ ) {
			DestroyDelete( m_chunksRendered[x][y] );
		}
		m_chunksRendered[x].clear();
	}
	m_chunksRendered.clear();
}

bool CChunkManager::populateChunks()
{
	for( unsigned int x = 0; x < m_chunksRendered.size(); x++ ) {
		for( unsigned int y = 0; y < m_chunksRendered[x].size(); y++ ) {
			if( !m_chunksRendered[x][y]->populateChunk() )
				return false;
		}
	}
	return true;
}

void CChunkManager::shiftChunks( glm::ivec2 movementVector )
{
	ChunkList newChunkList;
	std::vector<CChunk*> oldChunkLine;

	// Delete left column, load right column
	if( movementVector.x > 0 ) {
		// Get leftmost column
		oldChunkLine = m_chunksRendered[0];
		// Store columns without leftmost
		newChunkList.resize( m_chunksRenderedX );
		for( int x = 1; x < m_chunksRenderedX; x++ )
			newChunkList[x-1] = m_chunksRendered[x];
		// Move to rightmost, but reload
		newChunkList[m_chunksRenderedX-1] = oldChunkLine;
		// Recalculate positions
		for( int x = 0; x != m_chunksRenderedX; x++ ) {
			for( int y = 0; y != m_chunksRenderedY; y++ )
				newChunkList[x][y]->setRelativePosition( glm::ivec2( x, y ) );
		}
		m_chunksRendered = newChunkList;
		this->setChunksOrigin( this->getChunksOrigin() ); // realign
		for( auto it = newChunkList[m_chunksRenderedX-1].begin(); it != newChunkList[m_chunksRenderedX-1].end(); it++ )
			(*it)->populateChunk();
	}
	// Delete right column, load left column
	if( movementVector.x < 0 ) {
		// Get rightmost column
		oldChunkLine = m_chunksRendered[m_chunksRenderedX-1];
		// Store columns without rightmost
		newChunkList.resize( m_chunksRenderedX );
		for( int x = 0; x < m_chunksRenderedX-1; x++ )
			newChunkList[x+1] = m_chunksRendered[x];
		// Move to rightmost, but reload
		newChunkList[0] = oldChunkLine;
		// Recalculate positions
		for( int x = 0; x != m_chunksRenderedX; x++ ) {
			for( int y = 0; y != m_chunksRenderedY; y++ )
				newChunkList[x][y]->setRelativePosition( glm::ivec2( x, y ) );
		}
		m_chunksRendered = newChunkList;
		this->setChunksOrigin( this->getChunksOrigin() ); // realign
		for( auto it = newChunkList[0].begin(); it != newChunkList[0].end(); it++ )
			(*it)->populateChunk();
	}
	// Delete bottom row, load top row
	if( movementVector.y > 0 )
	{
		// Get the bottom row
		oldChunkLine.resize( m_chunksRenderedX );
		for( int x = 0; x < m_chunksRenderedX; x++ )
			oldChunkLine[x] = m_chunksRendered[x][0];
		// Store rows without bottom
		newChunkList.resize( m_chunksRenderedX );
		for( int x = 0; x < m_chunksRenderedX; x++ ) {
			newChunkList[x].resize( m_chunksRenderedY );
			for( int y = 1; y < m_chunksRenderedY; y++ ) {
				newChunkList[x][y-1] = m_chunksRendered[x][y];
			}
		}
		// Move bottom to top
		for( int x = 0; x < m_chunksRenderedX; x++ )
			newChunkList[x][m_chunksRenderedY-1] = oldChunkLine[x];
		// Recalculate positions
		for( int x = 0; x != m_chunksRenderedX; x++ ) {
			for( int y = 0; y != m_chunksRenderedY; y++ )
				newChunkList[x][y]->setRelativePosition( glm::ivec2( x, y ) );
		}
		m_chunksRendered = newChunkList;
		this->setChunksOrigin( this->getChunksOrigin() ); // realign
		for( auto it = oldChunkLine.begin(); it != oldChunkLine.end(); it++ ) {
			(*it)->populateChunk();
		}
	}
	// Delete top row, load bottom row
	if( movementVector.y < 0 )
	{
		// Get the top row
		oldChunkLine.resize( m_chunksRenderedX );
		for( int x = 0; x < m_chunksRenderedX; x++ )
			oldChunkLine[x] = m_chunksRendered[x][m_chunksRenderedY-1];
		// Store rows without top
		newChunkList.resize( m_chunksRenderedX );
		for( int x = 0; x < m_chunksRenderedX; x++ ) {
			newChunkList[x].resize( m_chunksRenderedY );
			for( int y = 1; y < m_chunksRenderedY; y++ ) {
				newChunkList[x][y] = m_chunksRendered[x][y-1];
			}
		}
		// Move top to bottom
		for( int x = 0; x < m_chunksRenderedX; x++ )
			newChunkList[x][0] = oldChunkLine[x];
		// Recalculate positions
		for( int x = 0; x != m_chunksRenderedX; x++ ) {
			for( int y = 0; y != m_chunksRenderedY; y++ )
				newChunkList[x][y]->setRelativePosition( glm::ivec2( x, y ) );
		}
		m_chunksRendered = newChunkList;
		this->setChunksOrigin( this->getChunksOrigin() ); // realign
		for( auto it = oldChunkLine.begin(); it != oldChunkLine.end(); it++ )
			(*it)->populateChunk();
	}
}
void CChunkManager::update( glm::ivec2 bottomLeft )
{
	glm::ivec2 bottomLeftChunk;
	glm::ivec2 movementVector;

	// Calculate the chunk the bottom left is in
	bottomLeftChunk = glm::ivec2( (int)floor(bottomLeft.x / CHUNK_WIDTH_UNITS), (int)floor(bottomLeft.y / CHUNK_WIDTH_UNITS) );
	// Fix rounding causing there to be 4 chunks at (0,0)
	if( bottomLeft.x < 0 )
		bottomLeftChunk.x--;
	if( bottomLeft.y < 0 )
		bottomLeftChunk.y--;
	// Check if we need to move the origin
	if( bottomLeftChunk != m_chunksOrigin ) {
		movementVector = bottomLeftChunk - m_chunksOrigin;
		this->setChunksOrigin( bottomLeftChunk );
		this->shiftChunks( movementVector );
	}
}

int CChunkManager::getChunksRenderedX() const {
	return m_chunksRenderedX;
}
void CChunkManager::setChunksRenderedX( int renderx ) {
	m_chunksRenderedX = renderx;
}
int CChunkManager::getChunkSRenderedY() const {
	return m_chunksRenderedY;
}
void CChunkManager::setChunksRenderedY( int rendery ) {
	m_chunksRenderedY = rendery;
}
glm::ivec2 CChunkManager::getChunksOrigin() const {
	return m_chunksOrigin;
}
void CChunkManager::setChunksOrigin( glm::ivec2 origin )
{
	for( unsigned int x = 0; x < m_chunksRendered.size(); x++ ) {
		for( unsigned int y = 0; y < m_chunksRendered[x].size(); y++ ) {
			m_chunksRendered[x][y]->setPosition( CChunkManager::RelativeToAbsolutePos( origin, m_chunksRendered[x][y] ) );
		}
	}
	m_chunksOrigin = origin;
}
CChunkManager::ChunkList& CChunkManager::getChunksRendered() {
	return m_chunksRendered;
}