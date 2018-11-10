


ExampleReusePage = ClassPanel("ExampleReusePage", '/Game/Example/ExampleReusePage/ExampleReusePageUI.ExampleReusePageUI')

function ExampleReusePage:construct()
    self.widget.Button_Close.OnClicked:Add(function() self:Close() end)
    self.widget.Button_ReloadCountRand.OnClicked:Add(function() self:ReloadCountRand() end)
end

function ExampleReusePage:destruct()
    self.widget.Button_Close.OnClicked:Clear()
    self.widget.Button_ReloadCountRand.OnClicked:Clear()
end

function ExampleReusePage:OnCreateItem(w)

end

function ExampleReusePage:OnReloadCountRand()
    --self.widget.ReusePageC.EventCreateItem:Add(function() self:ReloadCountRand() end)
end