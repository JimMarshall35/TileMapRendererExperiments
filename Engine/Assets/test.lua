
function GetTestViewModel()
	return {
		newButtonPressed = false,
		Get_NewButtonBackgroundSprite = function(self)
			print("Get_NewButtonBackgroundSprite")
			if self.newButtonPressed then 
				return "button_square" 
			else 
				return "button_square_depth"
			end
		end,
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