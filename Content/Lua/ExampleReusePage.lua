


ExampleReusePage = ClassPanel("ExampleReusePage", '/Game/ExampleReusePage/ExampleReusePageUI.ExampleReusePageUI')

function ExampleReusePage:construct()
    self.Button_Close = self.widget:FindWidget("Button_Close")
    self.Button_Close.OnClicked:Add(function() self:Close() end)
end

function ExampleReusePage:destruct()
    self.Button_Close.OnClicked:Clear()
end
