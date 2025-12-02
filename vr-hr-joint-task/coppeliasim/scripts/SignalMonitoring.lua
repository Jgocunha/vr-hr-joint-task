function resetSignals()
    sim.setInt32Signal('startSim', 0)--was 0
    sim.setInt32Signal('simStarted', 0)--was 0
    
    sim.setInt32Signal('object1', 0)
    sim.setInt32Signal('object2', 0)
    sim.setInt32Signal('object3', 0)
    sim.setInt32Signal('object4', 0)
    sim.setInt32Signal('object5', 0)
    sim.setInt32Signal('object6', 0)
    sim.setInt32Signal('object7', 0)
    sim.setInt32Signal('object8', 0)
    sim.setInt32Signal('createObjects', 0)
    sim.setInt32Signal('objectsCreated', 0)
    
    sim.setInt32Signal('targetObject', 0)
    
    sim.setInt32Signal('robotApproaching', 0)
    sim.setInt32Signal('robotGraspObj1', 0)
    sim.setInt32Signal('robotGraspObj2', 0)
    sim.setInt32Signal('robotGraspObj3', 0)
    sim.setInt32Signal('robotGraspObj4', 0)
    sim.setInt32Signal('robotGraspObj5', 0)
    sim.setInt32Signal('robotGraspObj6', 0)
    sim.setInt32Signal('robotGraspObj7', 0)
    sim.setInt32Signal('robotGraspObj8', 0)
    sim.setInt32Signal('robotPlaceObj1', 0)
    sim.setInt32Signal('robotPlaceObj2', 0)
    sim.setInt32Signal('robotPlaceObj3', 0)
    sim.setInt32Signal('robotPlaceObj4', 0)
    sim.setInt32Signal('robotPlaceObj5', 0)
    sim.setInt32Signal('robotPlaceObj6', 0)
    sim.setInt32Signal('robotPlaceObj7', 0)
    sim.setInt32Signal('robotPlaceObj8', 0)
    
    --features bijhouden
    --for i = 1, 8 do
   -- sim.setInt32Signal("ObjectIsLong_"..i, 0)
   -- end
   -- for i = 1, 8 do
    --sim.setInt32Signal("ObjectIsBlue_"..i, 0)
    --end
   
end


function sysCall_init()
    corout=coroutine.create(coroutineMain)
    resetSignals()
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
    resetSignals()
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
    sim.setInt32Signal('createObjects', 1)
    waitForSignal('startSim', 1)
    waitForSignal('simStarted', 1)
end

-- See the user manual or the available code snippets for additional callback functions and details
