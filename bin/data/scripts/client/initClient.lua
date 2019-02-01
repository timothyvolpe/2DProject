include( "cl_hud.lua" )

function Game:Initialize()
	print( "Client-sided lua initializing..." )
	print( "Resolution:"..Game:GetResolutionX() )
end