
function GetTestViewModel()
	return {
        textList = {},
        counter = 0,
        entryLine = "Type here",

        Get_entryLine = function(self)
            print(self.entryLine)
            print("Get entryLine called")
            return self.entryLine
        end,

        Set_entryLine = function(self, val)
            self.entryLine = val
            print(self.entryLine)
        end,

        onClearPress = function(self)
            self.textList = {}
            OnPropertyChanged(self, "stackpanelChildren")
        end,
        onAddPress = function(self)
            print("Add Pressed")
            table.insert(self.textList, {
                type = "text",
                content = "Label " .. tostring(self.counter),
                paddingLeft = "20.0",
                paddingTop = "20.0",
                colour = math.random(255) .. "," .. math.random(255) .. "," .. math.random(255) ..",255",
                children = {}
            })
            self.counter = self.counter + 1
            print(self.textList)
            print("len: " .. #self.textList)
            OnPropertyChanged(self, "stackpanelChildren")
        end,
        stackpanelChildren = function(self)
            return self.textList
        end

	}
end