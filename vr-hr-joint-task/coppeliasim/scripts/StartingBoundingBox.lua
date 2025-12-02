function sysCall_init()
    handle_rightController = sim.getObjectHandle('RightController')
    handle_startingBoundingBox = sim.getObjectHandle('StartingBoundingBox')
    sim.setInt32Signal('canBeRestarted', 0)
    sim.setInt32Signal('restart', 0)
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

function coroutineMain()
    local withinTheBoundingBoxBoundaries = false
    local prevWithinTheBoundingBoxBoundaries = false
    while true do
        -- Get the position of 'RightController' relative to the world frame
        local controllerPosition = sim.getObjectPosition(handle_rightController, -1)

        -- Get the position and orientation of 'StartingBoundingBox'
        local bboxPosition = sim.getObjectPosition(handle_startingBoundingBox, -1)
        local bboxOrientation = sim.getObjectOrientation(handle_startingBoundingBox, -1)
        
        -- Get the size of the bounding box (assuming it's a cuboid shape)
        local bboxSize = {0.1, 0.1, 0.3}

        -- Calculate the half-sizes for easier boundary checks
        local halfSize = {bboxSize[1] / 2, bboxSize[2] / 2, bboxSize[3] / 2}

        -- Transform controllerPosition to the bounding box's local frame
        -- This accounts for the bounding box's position and orientation
        local controllerPosInBBoxFrame = sim.multiplyVector(sim.buildMatrix(bboxPosition, bboxOrientation), controllerPosition)
        local bboxCenterInBBoxFrame = sim.multiplyVector(sim.buildMatrix(bboxPosition, bboxOrientation), bboxPosition)
        
        -- Check if the 'RightController' is within the bounding box boundaries
        if controllerPosInBBoxFrame[1] >= (bboxCenterInBBoxFrame[1] - halfSize[1]) and controllerPosInBBoxFrame[1] <= (bboxCenterInBBoxFrame[1] + halfSize[1]) and
           controllerPosInBBoxFrame[2] >= (bboxCenterInBBoxFrame[2] - halfSize[2]) and controllerPosInBBoxFrame[2] <= (bboxCenterInBBoxFrame[2] + halfSize[2]) and
           controllerPosInBBoxFrame[3] >= (bboxCenterInBBoxFrame[3] - halfSize[3]) and controllerPosInBBoxFrame[3] <= (bboxCenterInBBoxFrame[3] + halfSize[3]) then
            -- Position is inside the bounding box
            --sim.setInt32Signal('simStarted', 1) COMMENTED
            withinTheBoudningBoxBoundaries = true
        else
            withinTheBoudningBoxBoundaries = false
        end
        
        if withinTheBoudningBoxBoundaries == false and prevWithinTheBoundingBoxBoundaries == true then
            sim.setInt32Signal('simStarted', 1)
            -- Check if the simulation can be restarted
            if sim.getInt32Signal('canBeRestarted') == 1 then
                --sim.wait(1, false)
                sim.setInt32Signal('canBeRestarted', 0)
                sim.setInt32Signal('restart', 1)
                sim.setInt32Signal('simStarted', 1)
                sim.setInt32Signal('collisionCounter', 0)
            end
        end
        
        prevWithinTheBoundingBoxBoundaries = withinTheBoudningBoxBoundaries
        
        --if controllerPosInBBoxFrame[1] >= (bboxCenterInBBoxFrame[1] - halfSize[1]) and controllerPosInBBoxFrame[1] <= (bboxCenterInBBoxFrame[1] + halfSize[1]) and
        --   controllerPosInBBoxFrame[2] >= (bboxCenterInBBoxFrame[2] - halfSize[2]) and controllerPosInBBoxFrame[2] <= (bboxCenterInBBoxFrame[2] + halfSize[2]) and
        --   controllerPosInBBoxFrame[3] >= (bboxCenterInBBoxFrame[3] - halfSize[3]) and controllerPosInBBoxFrame[3] <= (bboxCenterInBBoxFrame[3] + halfSize[3]) then
            -- Check if the simulation can be restarted
        --    if sim.getInt32Signal('canBeRestarted') == 1 then
         --       sim.wait(1, false)
         ---       sim.setInt32Signal('canBeRestarted', 0)
          --      sim.setInt32Signal('restart', 1)
         --       sim.setInt32Signal('simStarted', 1)
          --      sim.setInt32Signal('collisionCounter', 0)
         --   end
        --end
    end
end

-- See the user manual or the available code snippets for additional callback functions and details
