
--Create an class.
function class(classname, url)
    local cls = {}
    cls.__classname = classname
    cls.__create    = function() return slua.loadUI(url) end
    cls.ctor        = function() end
    function cls.new(...)
        local obj = {}
        setmetatable(obj, cls)
        cls.__index = cls
        --obj.__class = cls
        obj.widget = cls.__create()
        obj.widget:SetLuaTable(obj)
        obj:ctor(...)
        return obj
    end
    function cls:del()
        if self.widget then
            self.widget:RemoveFromViewport()
            self.widget = nil
            for k,v in pairs(self) do
                --log(""..type(v).." "..tostring(v))
                if type(v) == "userdata" then
                    self[k] = nil
                end
            end
        end
    end
    return cls
end
