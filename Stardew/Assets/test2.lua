
function GetTestViewModel()
	return {
        textList = {},
        counter = 0,

        onAddPress = function(self)
            print("Add Pressed")
            table.insert(self.textList, {
                type = "text",
                content = "Label " .. tostring(self.counter),
                paddingLeft = "20.0",
                paddingTop = "20.0",
                colour = "0,0,0,255",
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