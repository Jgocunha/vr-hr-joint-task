 function sysCall_init()
    corout=coroutine.create(coroutineMain)
end

function sysCall_actuation()
    if coroutine.status(corout)~='dead' then
        local ok,errorMsg=coroutine.resume(corout)
        if errorMsg then
            error(debug.traceback(corout,errorMsg),2)
        end
    end
end

function arePositionsEqual(pos1, pos2, threshold)
    local dx = pos1[1] - pos2[1]
    local dy = pos1[2] - pos2[2]
    local dz = pos1[3] - pos2[3]
    local distance = math.sqrt(dx*dx + dy*dy + dz*dz)
    return distance < threshold
end

function areOrientationsEqual(orient1, orient2, threshold)
    local dx = orient1[1] - orient2[1]
    local dy = orient1[2] - orient2[2]
    local dz = orient1[3] - orient2[3]
    local distance = math.sqrt(dx*dx + dy*dy + dz*dz)
    return distance < threshold
end

function hopThroughConfigsObject(path, joints, reverse, dynModel, targetObject)
    local lb = sim.setThreadAutomaticSwitch(false)
    local s = 1
    local g = #path / 7
    local incr = 1
    if reverse then
        s = #path / 7
        g = 1
        incr = -1
    end
    for i = s, g, incr do
        -- Check if the target object has changed during execution
        --forceer object 1
        --if targetObject ~= 7 then
        if targetObject ~= sim.getInt32Signal('targetObject') then
            --sim.addStatusbarMessage('[hopThroughConfigsObject] - Target changed!')
            return false -- Target changed, exit early
        end
        
        if dynModel then
            for j = 1, #joints, 1 do
                sim.setJointTargetPosition(joints[j], path[(i - 1) * 7 + j])
            end
        else
            for j = 1, #joints, 1 do
                sim.setJointPosition(joints[j], path[(i - 1) * 7 + j])
            end
        end
        sim.switchThread()
    end
    sim.setThreadAutomaticSwitch(lb)
    return true
end
function hopThroughConfigs(path, joints, reverse, dynModel)
    local lb = sim.setThreadAutomaticSwitch(false)
    local s = 1
    local g = #path / 7
    local incr = 1
    if reverse then
        s = #path / 7
        g = 1
        incr = -1
    end
    for i = s, g, incr do
        if dynModel then
            for j = 1, #joints, 1 do
                sim.setJointTargetPosition(joints[j], path[(i - 1) * 7 + j])
            end
        else
            for j = 1, #joints, 1 do
                sim.setJointPosition(joints[j], path[(i - 1) * 7 + j])
            end
        end
        sim.switchThread()
    end
    sim.setThreadAutomaticSwitch(lb)
end

function waitForSignal(signalName, value)
    local signal = sim.getInt32Signal(signalName)
    while not (signal == value) do
        signal = sim.getInt32Signal(signalName)
        sim.addStatusbarMessage('Waiting for ' .. signalName .. '...')
        sim.wait(0.5)
    end
end

function setupRobotVars()
    simBase=sim.getObject('.')
    simTip=sim.getObject('./tip')
    connector = sim.getObjectHandle('./attachPoint')
    
    simJoints={}
    for i=1,7,1 do
        simJoints[i]=sim.getObject('./joint',{index=i-1})
    end
    

    sim.switchThread() -- make sure we have skipped the first simulation step
end

function calculateDistance(pos1, pos2)
    local dx = pos1[1] - pos2[1]
    local dy = pos1[2] - pos2[2]
    local dz = pos1[3] - pos2[3]
    local distance = math.sqrt(dx * dx + dy * dy + dz * dz)
    return distance
end

function calculatePathPoints(distance, globalVelocity)
    -- Calculate the travel time needed at the given velocity
    local travelTime = distance / globalVelocity
    -- Calculate the number of simulation steps needed
    local numSteps = travelTime / timeStep

    -- Ensure at least one point is used and round to nearest whole number
    return math.max(1, math.floor(numSteps + 0.5))
end

function moveRobotToPose(dynModel, targetObjectHandle, pathPointCount)
    local ikEnv=simIK.createEnvironment()
    local ikGroup=simIK.createGroup(ikEnv)
    local ikElement,simToIkMap=simIK.addElementFromScene(ikEnv,ikGroup,simBase,simTip,targetObjectHandle,simIK.constraint_pose)

    -- Retrieve some handles of objects created in the IK environment:
    local ikTip=simToIkMap[simTip]
    local ikJoints={}
    for i=1,#simJoints,1 do
        ikJoints[i]=simToIkMap[simJoints[i]]
    end

        -- Get the current position of the IK tip
    local currentIKTipPosition = sim.getObjectPosition(simTip, -1)

    -- Get the position of the target object
    local targetObjectPosition = sim.getObjectPosition(targetObjectHandle, -1)
    
    -- Calculate the distance between the IK tip and the target object
    local distance = calculateDistance(currentIKTipPosition, targetObjectPosition)
    local newPathPoints = calculatePathPoints(distance, pathPointCount)

    -- Generate a new path:
   path=simIK.generatePath(ikEnv,ikGroup,ikJoints,ikTip,newPathPoints)
   simIK.eraseEnvironment(ikEnv) -- Clean up the IK environment

    if #path > 0 then
        hopThroughConfigs(path, simJoints, false, dynModel) 
    else
        sim.addStatusbarMessage('Error computing IK path!')
        sim.switchThread() -- If no path, just yield control to keep simulation responsive
    end
end




 


function moveRobotToObject(dynModel, targetObjectHandle, pathPointCount, targetObject)
    local ikEnv=simIK.createEnvironment()
    local ikGroup=simIK.createGroup(ikEnv)
    local ikElement,simToIkMap=simIK.addElementFromScene(ikEnv,ikGroup,simBase,simTip,targetObjectHandle,simIK.constraint_pose)

    -- Retrieve some handles of objects created in the IK environment:
    local ikTip=simToIkMap[simTip]
    local ikJoints={}
    for i=1,#simJoints,1 do
        ikJoints[i]=simToIkMap[simJoints[i]]
    end
    
            -- Get the current position of the IK tip
    local currentIKTipPosition = sim.getObjectPosition(simTip, -1)

    -- Get the position of the target object
    local targetObjectPosition = sim.getObjectPosition(targetObjectHandle, -1)


    -- Calculate the distance between the IK tip and the target object
    local distance = calculateDistance(currentIKTipPosition, targetObjectPosition)
    local newPathPoints = calculatePathPoints(distance, pathPointCount)

    -- Generate a new path:
   path=simIK.generatePath(ikEnv,ikGroup,ikJoints,ikTip,newPathPoints)
   simIK.eraseEnvironment(ikEnv) -- Clean up the IK environment

    if #path > 0 then
        if not hopThroughConfigsObject(path, simJoints, false, dynModel, targetObject) then
            --sim.addStatusbarMessage('[moveRobotToPose] - Target changed!')
            return false -- Target changed, signal to adapt
        end
    else
        sim.addStatusbarMessage('Error computing IK path!')
        sim.switchThread() -- If no path, just yield control to keep simulation responsive
    end
    return true
end

function coroutineMain()
    setupRobotVars()
    velocityLargeMovements = 0.2 -- higher faster -- old 0.35
    velocitySmallMovements = 0.1 -- higher faster -- old 0.1
    gripperWaitTime = 0.2
    lastWaitTime = 0.01
    timeStep = 0.04 -- higher ->overall movements faster  -- old 0.04 -- 0.05

     dropZone = sim.getObjectHandle('dropZone')
     homePose = sim.getObjectHandle('homePose')
    dynModel=sim.isDynamicallyEnabled(simJoints[1])
    
    moveRobotToPose(dynModel, homePose, velocityLargeMovements)
    atHome = true

    sim.setInt32Signal('robotApproaching', 0)
    sim.setInt32Signal('robotGrasping', 0)

    for i = 1, 8 do
    sim.setInt32Signal('robotGraspObj'..i, 0)
    end
    for i = 1, 8 do
    sim.setInt32Signal('robotPlaceObj'..i, 0)
    end

    
    waitForSignal('startSim', 1)
    waitForSignal('simStarted', 1)
    sim.setInt32Signal('objectsCreated',8)
    
    considerHumanActionsFlag = sim.getInt32Signal('archType') 
   --considerHumanActionsFlag = 1
    sim.addStatusbarMessage('archType' .. considerHumanActionsFlag)
    if considerHumanActionsFlag == 0 then
        timeStep = 0.045
    end
    
    sim.setInt32Signal('robotApproaching', 1)
    sim.setInt32Signal('robotGrasping',0)
    
    rePlanningCount = 0
    prevTargetObj = 0
    commonGraspCount = 0
    while true do
        -- Adjust approach to potentially new target object dynamically
        while true do
            local objectPresent = false -- Track if any object is present
            local numberOfObjects = 8
            for i = 1, numberOfObjects do
                if sim.getInt32Signal('object' .. i) == 1 then
                    objectPresent = true -- An object is present
                end
            end
            if ((objectPresent == false) and (atHome == false)) then
                moveRobotToPose(dynModel, homePose, velocityLargeMovements)
                atHome = true
                rePlanningCount = 0
                prevTargetObj = 0
                commonGraspCount = 0
                sim.setInt32Signal('robotApproaching', 1)
                sim.setInt32Signal('robotGrasping',0)
                sim.setInt32Signal('gripperClosed', 0)
                sim.setInt32Signal('commonGrasp', commonGraspCount)
                sim.setInt32Signal('replanningCount', rePlanningCount)
            end
            targetObjectSignal = sim.getInt32Signal('targetObject')
            --targetObjectSignal = 7
            if not (targetObjectSignal == 0) then
                if prevTargetObj ~= targetObjectSignal then
                    rePlanningCount = rePlanningCount + 1
                    sim.addStatusbarMessage('[rePlanningCount] ' .. rePlanningCount)
                    sim.setInt32Signal('replanningCount',rePlanningCount) 
                end
                prevTargetObj = targetObjectSignal
                atHome = false
                dynModel=sim.isDynamicallyEnabled(simJoints[1])
                targetObject = 'object_' .. targetObjectSignal
                dummyObject = 'dummy_' .. targetObjectSignal
                targetObjectHandle = sim.getObjectHandle(targetObject)
                targetDummyHandle = sim.getObjectHandle(dummyObject)
                sim.addStatusbarMessage('Target object is ' .. targetObject .. " with handle " .. targetObjectHandle)
                sim.setInt32Signal('robotApproaching', 1)
                sim.setInt32Signal('robotGrasping',0)
                
                
                                        
                local isLong = sim.getInt32Signal("ObjectIsLong_"..targetObjectSignal)
                if isLong == 1 then
                        sim.addStatusbarMessage("Target object is LONG -> afwijkend gedrag")
                        -- bv. aanpassing snelheid, gripper opening, etc.
                        velocityLargeMovements = 0.1 -- higher faster -- old 0.35
                        velocitySmallMovements = 0.04 
                        
                end
                
                
                 -- Check if the object is still present and not being grasped by a human
                local objectPresent = sim.getInt32Signal('object' .. targetObjectSignal)
                if (considerHumanActionsFlag == true) then
                    humanNotGrasping = sim.getInt32Signal('humanGraspObj' .. targetObjectSignal)
                else
                    humanNotGrasping = 0
                end
                if objectPresent ~= 1 or humanNotGrasping ~= 0 then
                    sim.addStatusbarMessage('Object not present or already being grasped by a human, waiting for new target...')
                    sim.wait(lastWaitTime)
                    break -- Exit the current iteration and wait for a new targetObjectSignal
                end
                
                if not moveRobotToObject(dynModel, targetDummyHandle, velocityLargeMovements, targetObjectSignal) then
                    sim.addStatusbarMessage('[coroutineMain] - Target changed while approaching!')
                else
                     -- Check if the object is still present and not being grasped by a human
                    local objectPresent = sim.getInt32Signal('object' .. targetObjectSignal)
                    if (considerHumanActionsFlag == 1) then
                        humanNotGrasping = sim.getInt32Signal('humanGraspObj' .. targetObjectSignal)
                    else
                        humanNotGrasping = 0
                    end
                    if objectPresent ~= 1 or humanNotGrasping ~= 0 then
                        sim.addStatusbarMessage('Object not present or already being grasped by a human, waiting for new target...')
                        sim.wait(lastWaitTime)
                        break -- Exit the current iteration and wait for a new targetObjectSignal
                    end
                    -- proceed to try and pick up
                    if not moveRobotToObject(dynModel, targetObjectHandle, velocitySmallMovements, targetObjectSignal) then
                        sim.addStatusbarMessage('[coroutineMain] - Target changed while trying to grasp!')
                        moveRobotToPose(dynModel, targetDummyHandle, velocitySmallMovements)
                    else
                        sim.setInt32Signal('robotApproaching', 0)
                        sim.setInt32Signal('robotGrasping', 1)

                        
                        
                        -- Check if the object is still present and not being grasped by a human
                        local objectPresent = sim.getInt32Signal('object' .. targetObjectSignal)
                        if (considerHumanActionsFlag == 1) then
                            humanNotGrasping = sim.getInt32Signal('humanGraspObj' .. targetObjectSignal)
                        else
                            humanNotGrasping = 0
                        end
                        if objectPresent ~= 1 or humanNotGrasping ~= 0 then
                            sim.addStatusbarMessage('Object not present or already being grasped by a human, waiting for new target...')
                            sim.setInt32Signal('robotGrasping',0)        

                            moveRobotToPose(dynModel, targetDummyHandle, velocitySmallMovements)
                            break -- Exit the current iteration and wait for a new targetObjectSignal
                        end
                        
                                             
                        if isLong == 1 then
                            sim.addStatusbarMessage("Long object wait for human grip...")

                            while sim.getInt32Signal('humanGraspObj'..targetObjectSignal) ~= 1 do
                                local stillPresent = sim.getInt32Signal('object' .. targetObjectSignal)
                                if stillPresent ~= 1 then
                                    break
                                end
                                sim.wait(0.05)
                            end

                        
                        end
                        sim.setInt32Signal('gripperClosed', 1)
                        --if (considerHumanActionsFlag == 0) then
                        if sim.getObjectParent(targetObjectHandle) ~= -1 then
                            sim.setObjectParent(targetObjectHandle, -1, true)
                            sim.setObjectInt32Parameter(targetObjectHandle, sim.shapeintparam_respondable, 1)
                            sim.setObjectInt32Param(targetObjectHandle, sim.shapeintparam_static, 0)
                            sim.resetDynamicObject(targetObjectHandle)
                            sim.wait(0.1)
                            commonGraspCount = commonGraspCount + 1
                            sim.setInt32Signal('commonGrasp', commonGraspCount)
                        end
                        sim.setObjectParent(targetObjectHandle, connector, true)
                        sim.setObjectInt32Parameter(targetObjectHandle, sim.shapeintparam_respondable, 0)
                        --sim.setObjectInt32Param(targetObjectHandle, sim.shapeintparam_static, 1)
                        --sim.resetDynamicObject(targetObjectHandle)
                        sim.wait(gripperWaitTime)
                        --sim.setObjectParent(targetObjectHandle, connector, true)
                        --sim.setObjectInt32Parameter(targetObjectHandle, sim.shapeintparam_respondable, 0)
                         -- Set signals based on targetObjectHandle
                        if targetObjectSignal == 1 then 
                            sim.setInt32Signal('robotGraspObj1', 1)
                        elseif targetObjectSignal == 2 then 
                            sim.setInt32Signal('robotGraspObj2', 1)
                        elseif targetObjectSignal == 3 then 
                            sim.setInt32Signal('robotGraspObj3', 1)
                        elseif targetObjectSignal == 4 then 
                            sim.setInt32Signal('robotGraspObj4', 1)
                        elseif targetObjectSignal == 5 then 
                            sim.setInt32Signal('robotGraspObj5', 1)
                        elseif targetObjectSignal == 6 then 
                            sim.setInt32Signal('robotGraspObj6', 1)
                        elseif targetObjectSignal == 7 then 
                            sim.setInt32Signal('robotGraspObj7', 1)
                        elseif targetObjectSignal == 8 then 
                            sim.setInt32Signal('robotGraspObj8', 1)
                        end

                        moveRobotToPose(dynModel, targetDummyHandle, velocitySmallMovements)            
                        
                        moveRobotToPose(dynModel, dropZone, velocityLargeMovements)
                        sim.setInt32Signal('gripperClosed', 0)
                        sim.wait(0.05)
                        sim.setObjectParent(targetObjectHandle, -1, true)
                        sim.setObjectInt32Parameter(targetObjectHandle, sim.shapeintparam_respondable, 1)
                        sim.setObjectInt32Param(targetObjectHandle, sim.shapeintparam_static, 0)
                        sim.resetDynamicObject(targetObjectHandle)
                        sim.wait(gripperWaitTime)
                        -- Set signals based on targetObjectHandle
                        if targetObjectSignal == 1 then 
                            sim.setInt32Signal('robotPlaceObj1', 1)
                        elseif targetObjectSignal == 2 then 
                            sim.setInt32Signal('robotPlaceObj2', 1)
                        elseif targetObjectSignal == 3 then 
                            sim.setInt32Signal('robotPlaceObj3', 1)
                        elseif targetObjectSignal == 4 then 
                            sim.setInt32Signal('robotPlaceObj4', 1)
                        elseif targetObjectSignal == 5 then 
                            sim.setInt32Signal('robotPlaceObj5', 1)
                        elseif targetObjectSignal == 6 then 
                            sim.setInt32Signal('robotPlaceObj6', 1)
                        elseif targetObjectSignal == 7 then 
                            sim.setInt32Signal('robotPlaceObj7', 1)
                        elseif targetObjectSignal == 8 then 
                            sim.setInt32Signal('robotPlaceObj8', 1)
                        end
                        sim.setInt32Signal('robotGrasping', 0)
                    end
                end
            end
            sim.wait(lastWaitTime)
        end
    end
end

function sysCall_cleanup() 
end 

