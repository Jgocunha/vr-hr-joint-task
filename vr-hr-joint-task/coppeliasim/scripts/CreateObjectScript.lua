function createObjectData()
    -- Create a table to store pose variables for each object
    objectPoses = {}
    
    -- Loop to set simInt32signal for each object
    for objectCounter = 1, numberOfObjects do        
        -- Create pose variables for each object
        objectPoses["object_"..objectCounter] = {
            position = {0, 0, 0}, -- Initialize with default position
            orientation = {0, 0, 0} -- Initialize with default orientation
        }
    end
end

function defineWorkspaceBounds()
    --Define workspace bounds
    min_x = -0.25; -- Minimum x-coordinate
    max_x = 0.75;  -- Maximum x-coordinate
    min_y = -0.50; -- Minimum y-coordinate
    max_y = 0.45;  -- Maximum y-coordinate
    min_z = 0.60;  -- Minimum z-coordinate
    max_z = 0.80;  -- Maximum z-coordinate
    min_alpha = 0.45
    max_alpha = 0.55
    min_beta = 0.45
    max_beta = 0.55
    min_gamma = 0.45
    max_gamma = 0.55
end

function setObjectSignalsTo(value)
    -- Loop to set simInt32signal for each object
    for objectCounter = 1, numberOfObjects do
        sim.setInt32Signal("object"..objectCounter, value) 
    end
end

function printObjectData()
    for objectName, pose in pairs(objectPoses) do
        print("Object: " .. objectName)
        print("Object exists: " .. sim.getInt32Signal(objectName))
        
        local objectHandle = objectHandles[tonumber(objectName:match("%d+$"))]
        if objectHandle ~= nil then
            print("Object handle: " .. objectHandle)
            print("Is object in workspace: " .. tostring(isObjectInWorkspace(pose)))
        else
            print("Object handle: not available")
            print("Is object in workspace: N/A")
        end
        
        print("Position: {" .. table.concat(pose.position, ", ") .. "}")
        print("Orientation: {" .. table.concat(pose.orientation, ", ") .. "}")
    end
end

function isObjectInWorkspace(pose)
    local inPositionBounds = pose[1] >= min_x and pose[1] <= max_x and
                             pose[2] >= min_y and pose[2] <= max_y and
                             pose[3] >= min_z and pose[3] <= max_z
                             
    local inOrientationBounds = pose[4] >= min_alpha and pose[4] <= max_alpha and
                                 pose[5] >= min_beta and pose[5] <= max_beta and
                                 pose[6] >= min_gamma and pose[6] <= max_gamma

    return inPositionBounds and inOrientationBounds
end


function waitForSignal(signalName, value)
    local signal = sim.getInt32Signal(signalName)
    while not (signal == value) do
        signal = sim.getInt32Signal(signalName)
        sim.addStatusbarMessage('Waiting for ' .. signalName .. '...')
        sim.wait(0.5)
    end
end

function createObjects()
    startY = 0.4
  
    local distanceY = 0.115
    objectHandles = {} -- Table to store object handles
    
    for i = 1, numberOfObjects do
        local boxSize = 0.05
        local sizeX = boxSize
        local sizeY = boxSize + 0.06
        local sizeZ = boxSize
        --local position = {0.0, -0.025 - (i-1) * distanceY, 0.8}
        local position = {0.02, startY - (i-1) * distanceY, 0.68}
        local shapeName = "object_" .. i
        
        --size aanmaken
        local ObjectIsLong = (i == 1 or i == 2 or i == 5 or i == 6)
        if ObjectIsLong then
            sizeZ = boxSize + 0.08 
            position[1] = position[1]+0.02
        end
        
        --kleur kiezen
        local ObjectIsBlue = (i > 4)
        
        
        local boxHandle = sim.createPureShape(0, 16, {sizeX, sizeY, sizeZ}, 0.1)
        sim.setInt32Signal("ObjectIsLong_"..i, ObjectIsLong and 1 or 0)
        sim.setInt32Signal("ObjectIsBlue_"..i, ObjectIsBlue and 1 or 0)
        --print("DEBUG: Object " .. i .. " ObjectIsBlue = " .. tostring(ObjectIsBlue))
        --print("DEBUG: Object " .. i .. " ObjectIsLong = " .. tostring(ObjectIsLong))
        
    
        local color
        if ObjectIsBlue then
            color = {0,0,1}
            else
            color = {1,1,0}
        end
        sim.setShapeColor(boxHandle, nil, sim.colorcomponent_ambient_diffuse,color)
        sim.setObjectPosition(boxHandle, -1, position)
        sim.setObjectAlias(boxHandle, shapeName)
        
    

    
        local desiredOrientation = {math.rad(0), math.rad(90), math.rad(90)}
        sim.setObjectOrientation(boxHandle, -1, desiredOrientation)
        sim.setObjectInt32Parameter(boxHandle, sim.shapeintparam_respondable, 1)
        sim.setObjectInt32Parameter(boxHandle, sim.shapeintparam_static, 0)
        
        -- Save the object handle
        objectHandles[i] = boxHandle
        position[1] = position[1] - 0.1
        position[3] = position[3] + 0.05
        local dummyName = "dummy_" .. i
        local dummy = sim.createDummy(0.001)
        
        local dummyPos = {position[1], position[2], position[3]}
        
        
        if ObjectIsLong then
           dummyPos[1] = dummyPos[1] + 0.05   -- ? Grijppunt 3 cm naar voren
        end
        
        sim.setObjectPosition(dummy, -1, dummyPos)
        sim.setObjectAlias(dummy, dummyName)
        sim.setObjectOrientation(dummy, -1, desiredOrientation)
        
        sim.switchThread()
    end
    sim.setInt32Signal('objectsCreated', 8) --aangepast van 1 naar x
end

function updateObjectPoses()
    for objectName, pose in pairs(objectPoses) do
        local objectHandle = sim.getObjectHandle(objectName)
        if objectHandle ~= -1 then
            pose.position = sim.getObjectPosition(objectHandle, -1)
            pose.orientation = sim.getObjectOrientation(objectHandle, -1)
        end
    end
end

function updateAreObjectsInTheWorkspace()
    areObjectsInWorkspace = {} -- Initialize array to store whether objects are in the workspace
    
    for i, objectHandle in ipairs(objectHandles) do
        local objectPose = sim.getObjectPose(objectHandle, -1)
        areObjectsInWorkspace[i] = isObjectInWorkspace(objectPose)
    end
end


function sysCall_init()
    corout = coroutine.create(coroutineMain)
    end

function sysCall_actuation()
    if coroutine.status(corout) ~= 'dead' then
        local ok, errorMsg = coroutine.resume(corout)
        if errorMsg then
            error(debug.traceback(corout, errorMsg), 2)
        end
    end
end


function sysCall_cleanup()
    -- do some clean-up here
end


function checkObjectPresenceAndPosition()
    local objectPresent = false -- Track if any object is present

    for i = 1, numberOfObjects do
        local objectSignal = sim.getInt32Signal('object' .. i)
        if objectSignal == -1 then
            break  -- Signal does not exist, exit loop
        end

        objectPresent = true -- An object is present

        -- Get handle to the object
        local object_handle = sim.getObjectHandle('object_' .. i)

        -- Check if the signal is set for the object
        if object_handle ~= -1 then
            -- Get the position of the object
            local object_pose = sim.getObjectPose(object_handle, -1)

            -- Check if the object is in the workspace
            local in_workspace = isObjectInWorkspace(object_pose)

            sim.setInt32Signal('object' .. i, in_workspace and 1 or 0)
        end
    end
end

function detectIllegalObjMovements()
    for i = 1, numberOfObjects do
        local objHasntBeenPlaced = sim.getInt32Signal('humanPlaceObj' .. i) == 0 or sim.getInt32Signal('robotPlaceObj' .. i) == 0
        local objOutOfWorkspace = sim.getInt32Signal('object' .. i) == 0

        if objHasntBeenPlaced then
            if objOutOfWorkspace then
                local objIsGrasped = sim.getInt32Signal('humanGraspObj' .. i) == 1 or sim.getInt32Signal('robotGraspObj' .. i) == 1
                if objIsGrasped then
                    -- Legal movement
                    --print("Object " .. i .. " is being moved within grasp.")
                else
                    -- Illegal movement
                    print("Object " .. i .. " is out of workspace and not grasped.")
                    -- Reset object to initial position and orientation
                    resetObject(i)
                end
            else
                -- Legal movement
                --print("Object " .. i .. " is being moved within workspace.")
            end
        end
    end
end

function resetObject(objectIndex)
    local position = {0.0, startY - (objectIndex-1) * 0.125, 0.68}
    local orientation = {math.rad(0), math.rad(90), math.rad(90)}
    local object_handle = sim.getObjectHandle('object_' .. objectIndex)
    if object_handle ~= -1 then
        sim.setObjectPosition(object_handle, -1, position)
        sim.setObjectOrientation(object_handle, -1, orientation)
        -- Optionally reset other properties
        sim.setObjectParent(object_handle, -1, true)
        sim.setObjectInt32Parameter(object_handle, sim.shapeintparam_respondable, 1)
        sim.setObjectInt32Parameter(object_handle, sim.shapeintparam_static, 0)
        sim.setInt32Signal('object'..objectIndex, 1)
    end
end


function coroutineMain()
    numberOfObjects = 8
    
    defineWorkspaceBounds()
    setObjectSignalsTo(0)
    createObjectData()
    createObjects()
    setObjectSignalsTo(1)
    updateObjectPoses()
    updateAreObjectsInTheWorkspace()
    
    while(true) do
        updateObjectPoses()
        updateAreObjectsInTheWorkspace()
        checkObjectPresenceAndPosition()
        detectIllegalObjMovements()
        -- Check if no objects are present
        local objectPresent = false -- Track if any object is present
        local objectCounter = 0

        for i = 1, numberOfObjects do
            if sim.getInt32Signal('object' .. i) == 1 then
                objectPresent = true -- An object is present
            end
            if sim.getInt32Signal('object' .. i) == 0 then
                objectCounter =objectCounter +1
            end
        end

        -- Check if no object is present and 'simStarted' signal is set
        if objectCounter >=4 and sim.getInt32Signal('robotGrasping') == 0 
            and sim.getInt32Signal('simStarted') == 1 then
            -- Reset signals and allow restart
            sim.setInt32Signal('canBeRestarted', 1)
            sim.setInt32Signal('simStarted', 0)
            sim.setInt32Signal('robotApproaching', 0)
            sim.setInt32Signal('gatherIdleTime', 0)
            
            --for i = 1, numberOfObjects do
            --    sim.setInt32Signal('robotGraspObj'..i, 0)
            --    sim.setInt32Signal('robotPlaceObj'..i, 0)
            --    sim.setInt32Signal('humanGraspObj'..i, 0)
            --    sim.setInt32Signal('humanPlaceObj'..i, 0)
            --end
        end

        -- Check if no object present and 'restart' signal is set
        if objectCounter >=4 and sim.getInt32Signal('restart') == 1 then
            -- Reset 'restart' signal and objects' positions
            sim.setInt32Signal('restart', 0)
            sim.setInt32Signal('simStarted', 1)
        
            for i = 1, numberOfObjects do
                sim.setInt32Signal('robotGraspObj'..i, 0)
                sim.setInt32Signal('robotPlaceObj'..i, 0)
                sim.setInt32Signal('humanGraspObj'..i, 0)
                sim.setInt32Signal('humanPlaceObj'..i, 0)
            end
            sim.setInt32Signal('canBeRestarted', 0) 
            sim.setInt32Signal('gatherIdleTime', 1)
            shapeCounter = 1
            for i = 1, numberOfObjects do
                local position = {0.6, startY - (i-1) * 0.125, 0.68}
                local orientation = {math.rad(0), math.rad(90), math.rad(90)}
                local object_handle = sim.getObjectHandle('object_' .. i)
                if object_handle ~= -1 then
                    sim.setObjectPosition(object_handle, -1, position)
                    sim.setObjectOrientation(object_handle, -1, orientation)
                    sim.setObjectInt32Parameter(object_handle, sim.shapeintparam_respondable, 1)
                    sim.setObjectInt32Parameter(object_handle, sim.shapeintparam_static, 0)
                    sim.setInt32Signal('object'..shapeCounter, 1)
                    shapeCounter = shapeCounter + 1
                end
            end
        end
    end
end
