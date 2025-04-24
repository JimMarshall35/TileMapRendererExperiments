
function GetTestViewModel()
	return {
		-- random data members
		newButtonPressed = false,
		
		-- a one way binding property called NewButtonBackgroundSprite
		Get_NewButtonBackgroundSprite = function(self)
			if self.newButtonPressed then 
				return "button_square" 
			else 
				return "button_square_depth"
			end
		end,

		-- event handlers that can be bound to individual widgets
		OnNewGameMouseDown = function(self, x, y, button)
			if not self.newButtonPressed then
				self.newButtonPressed = true			
				OnPropertyChanged(self, "NewButtonBackgroundSprite")
			end
		end,
		OnNewGameMouseLeave = function (self, x, y)
			if self.newButtonPressed then
				self.newButtonPressed = false			
				OnPropertyChanged(self, "NewButtonBackgroundSprite")
			end
		end,
		OnNewGameMouseUp = function(self, x, y, button)
			if self.newButtonPressed then
				self.newButtonPressed = false			
				OnPropertyChanged(self, "NewButtonBackgroundSprite")
			end
		end
	}
end