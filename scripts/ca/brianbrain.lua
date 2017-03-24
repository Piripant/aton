SCRIPT_TYPE = "CA"

NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1} }
STATES_NUM = 3

alive = 2
dying = 1
dead = 0

function get_next_state (neighbors, state)
    local alive_cells = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == alive then
            alive_cells = alive_cells + 1
        end
    end
    
    if state == dead then
        if alive_cells == 2 then
            return alive
        end
    elseif state == alive then
        return dying
    elseif state == dying then
        return dead
    end
    
end