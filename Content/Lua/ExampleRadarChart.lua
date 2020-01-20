


ExampleRadarChart = ClassPanel("ExampleRadarChart", '/Game/Example/ExampleRadarChart/ExampleRadarChart.ExampleRadarChart')

function ExampleRadarChart:construct()
    self.Button_Close = self.widget:FindWidget("Button_Close")
    self.Button_Close.OnClicked:Add(function() self:Close() end)
end

function ExampleRadarChart:destruct()
    self.Button_Close.OnClicked:Clear()
end
