
function GetTestViewModel()
	return {
		OnNewGameMouseDown = function(x,y,button)
			-- body
			print(x,y,button)
			print("lua, OnNewGamePress!!\n")
		end
	}
end