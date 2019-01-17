include( "cl_hud.lua" )

print( "Hello from the Client!" )

print( "Are we server: " .. tostring( SERVER ) )
print( "Are we client: " .. tostring( CLIENT ) )

function Game:Initialize()
	print( "\tScript Initialize..." )
	
	local x = Game:GetResolutionX()
	local y = Game:GetResolutionY()
	print( "Resolution is "..x.."x"..y )
	
	local fullscreen = Game:IsFullscreen()
	local borderless = Game:IsBorderless()
	print( "Game is fullscreen: "..tostring( fullscreen )..", game is borderless: "..tostring( borderless ) )
end