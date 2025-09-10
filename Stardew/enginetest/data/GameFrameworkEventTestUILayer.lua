function GetTestViewModel()
	return {
		OnXMLUILayerPush = function(self)
			FireGameFrameworkEvent({ 
				prop="val",
				intProp=42,
				boolProp=false,
				floatProp=32.5
			}, "OnGameHUDPush")
			print("LOLOLOLOLOLOLOLOLOLOLOOL HERE HERE HERE")
		end,
		OnXMLUILayerPop = function(self)
			print("LOLOLOLOLOLOLOLOLOLOLOOL POP")
		end
	}
end