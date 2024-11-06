local Meta = {
    _currentUniqueId = 0
}
Meta.__index = Meta
Meta.__newindex = function() error(`__newindex is locked`) end
local Storage = setmetatable({}, Meta)

function Meta:store(obj: unknown): number
    Meta._currentUniqueId += 1
    rawset(Storage::any, Meta._currentUniqueId, obj)
    return Meta._currentUniqueId
end
function Meta:free(id: number)
    rawset(Storage::any, id, nil)
end

function Meta:clear()
    table.clear(Storage::any)
end

function Meta:__add(obj: unknown): typeof(Storage)
    Storage:store(obj)
    return Storage
end

return Storage 
