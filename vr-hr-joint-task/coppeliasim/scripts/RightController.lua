-- lua
--100

-- Function to calculate the distance of the hand to the middle line of the table
function calculateDistanceToObjects(handPosition)
    -- Table center and dimensions
    local tableCenterX = 0.6
    local tableCenterZ = 0.631 + 0.1

    -- Calculate the distance in the X dimension
    local distanceX = math.abs(handPosition[1] - tableCenterX)
    -- Calculate the distance in the Z dimension
    local distanceZ = math.abs(handPosition[3] - tableCenterZ)
    -- Calculate the total distance (Euclidean distance in X-Z plane)
    local distance = math.sqrt(distanceX * distanceX + distanceZ * distanceZ)

    return distance
end

-- Function to invert the distance to represent the closeness
function calculateClosenessToObjects(distance, safeZone)
    -- Ensure distance is always greater than zero to avoid division by zero
    distance = math.max(distance, safeZone)
    -- Invert the distance
    return 1.0 / distance
end

-- Function to normalize hand's Y position to a 0-90 scale
function normalizeHandPosition(pos_y)
    -- Define the min and max of the table in Y dimension
    local yMin = -0.4
    local yMax = 0.1

    -- Define the min and max of the scale
    local scaleMin = 0
    local scaleMax = 50

    -- Normalize pos_y to the 0-50 scale
    local normalizedScale = scaleMin + (scaleMax - scaleMin) * (pos_y - yMin) / (yMax - yMin)
    
    return normalizedScale
end


function sysCall_init()
    --sim = require('sim')
    handle = sim.getObjectHandle('RightController')
    -- do some initialization here
    sim.setInt32Signal("humanGraspObj1", 0)
    sim.setInt32Signal("humanGraspObj2", 0)
    sim.setInt32Signal("humanGraspObj3", 0)
    sim.setInt32Signal("humanGraspObj4", 0)
    sim.setInt32Signal("humanGraspObj5", 0)
    sim.setInt32Signal("humanGraspObj6", 0)
    sim.setInt32Signal("humanGraspObj7", 0)
    sim.setInt32Signal("humanGraspObj8", 0)
    
    sim.setInt32Signal("humanPlaceObj1", 0)
    sim.setInt32Signal("humanPlaceObj2", 0)
    sim.setInt32Signal("humanPlaceObj3", 0)
    sim.setInt32Signal("humanPlaceObj4", 0)
    sim.setInt32Signal("humanPlaceObj5", 0)
    sim.setInt32Signal("humanPlaceObj6", 0)
    sim.setInt32Signal("humanPlaceObj7", 0)
    sim.setInt32Signal("humanPlaceObj8", 0)
    
end

function sysCall_actuation()
    -- put your actuation code here
    --local position = sim.getObjectPosition(handle, -1)
    --sim.addStatusbarMessage('Controller position x=' .. position[1] .. ' y=' .. position[2] .. ' z=' .. position[3])
    
   -- local safeZone = 0.01;
    --local handProximity = calculateClosenessToObjects(calculateDistanceToObjects(position), safeZone)
    --local normalizedHandY = normalizeHandPosition(position[2])

    -- Check if normalized hand position is outside the 0-90 range
    --if normalizedHandY < 0 or normalizedHandY > 50 then
    --   normalizedHandY = -1
    --end

    -- If normalized hand position is -1, set hand proximity to 0
   -- if normalizedHandY == -1 then
    --    handProximity = 0
    --end
    
    

    --sim.setFloatSignal('hand_proximity', handProximity)
    --sim.setFloatSignal('hand_y', normalizedHandY)
end

function sysCall_sensing()
    -- put your sensing code here
end

function sysCall_cleanup()
    -- do some clean-up here
end

-- See the user manual or the available code snippets for additional callback functions and details

