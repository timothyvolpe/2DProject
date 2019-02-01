
local testInterface = Interface.new( "Label", "testLabel", Interface.GetControl( "DebugScreends" ) )

if( testInterface == nil ) then
	print( "Interface is nil!" )
end

testInterface:activate()

Interface.GetControl( "testLabel" )