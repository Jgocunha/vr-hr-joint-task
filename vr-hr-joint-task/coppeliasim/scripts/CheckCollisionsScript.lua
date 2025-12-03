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

function getSize(handle)
    return {0.065, 0.15, 0.08}
end

function checkCollision(robotHandle, humanHandle)
    -- Get positions and sizes
    local robotPosition = getPosition(robotHandle)
    local robotSize = getSize(robotHandle)
    
    local humanPosition = getPosition(humanHandle)
    local humanSize = getSize(humanHandle)
    
    -- Check for collision
    local collision = false
    if math.abs(robotPosition[1] - humanPosition[1]) < (robotSize[1] + humanSize[1]) / 2 and
       math.abs(robotPosition[2] - humanPosition[2]) < (robotSize[2] + humanSize[2]) / 2 and
       math.abs(robotPosition[3] - humanPosition[3]) < (robotSize[3] + humanSize[3]) / 2 then
        collision = true
    end
    
    return collision
end

function coroutineMain()
    local robotHandle = getHandle('DummyRobot')
    local humanHandle = getHandle('DummyHuman')
    sim.setInt32Signal('collisionCounter', 0)
    local prevState = 0
    local collisionCounter = 0
    while true do
        -- Get handles
        collisionCounter = sim.getInt32Signal('collisionCounter')

        if robotHandle ~= -1 and humanHandle ~= -1 then
            -- Check collision
            local isColliding = checkCollision(robotHandle, humanHandle)
            
            
            if sim.getInt32Signal('robotApproaching') == 1 then
                if isColliding then
                    if prevState == 0 then
                        collisionCounter = collisionCounter + 1
                        print("Collision detected! " .. collisionCounter)
                        sim.setInt32Signal('collisionCounter', collisionCounter)
                        prevState = 1
                    end
                end
            else
                prevState = 0
            end
        end
        sim.wait(0.1)
        -- Insert a small delay to avoid unnecessary CPU load
    end
end
