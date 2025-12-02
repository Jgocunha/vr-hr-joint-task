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

function sysCall_cleanup()
    -- do some clean-up here
end

function getHandle(name)
    return sim.getObjectHandle(name)
end

function getPosition(handle)
    return sim.getObjectPosition(handle, -1)
end

function isInTheSamePlace(initialPosition, currentPosition)
    if math.abs(initialPosition[1] - currentPosition[1]) < 0.0025 and
       math.abs(initialPosition[2] - currentPosition[2]) < 0.0025 and
       math.abs(initialPosition[3] - currentPosition[3]) < 0.0025 then
        return true
    end
    return false
end

function waitForSignal(signalName, value)
    local signal = sim.getInt32Signal(signalName)
    while not (signal == value) do
        signal = sim.getInt32Signal(signalName)
        sim.addStatusbarMessage('Waiting for ' .. signalName .. '...')
        sim.wait(0.5)
    end
end

function coroutineMain()
    
    -- Get handles
    local rightControllerHandle = sim.getObjectHandle('/HTC_VIVE/RightController')
    local tipHandle = getHandle('/Franka/tip')
    local idleTimeHand = 0
    local idleTimeRobot = 0
    
    local handPositionPrev = getPosition(rightControllerHandle)
    local robotTipPositionPrev = getPosition(tipHandle)
    
    waitForSignal('startSim', 1)
    waitForSignal('simStarted', 1)
    sim.setInt32Signal('gatherIdleTime', 1)
    while true do
        while sim.getInt32Signal('gatherIdleTime') == 1 do
            handPosition = getPosition(rightControllerHandle)
            tipPosition = getPosition(tipHandle)
            
            if(isInTheSamePlace(handPositionPrev,handPosition)) then
                idleTimeHand = idleTimeHand + 1
            end
            if(isInTheSamePlace(robotTipPositionPrev,tipPosition)) then
                idleTimeRobot = idleTimeRobot + 1
            end
            handPositionPrev = handPosition
            robotTipPositionPrev = tipPosition
            
            sim.setInt32Signal('humanIdleTime', idleTimeHand)
            sim.setInt32Signal('robotIdleTime', idleTimeRobot)
            sim.wait(1, false)
        end
        idleTimeHand = 0
        idleTimeRobot = 0
        sim.wait(0.1)
    end
end
