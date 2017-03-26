SCRIPT_TYPE = "CA"
TAGS = { 
    {0, "Empty"}, 
    {1, "Flooded"} 
}

NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATES_NUM = 2

function get_next_state (neighbors, state)
    if state == 1 then
        return 1
    end
    
    local alive_cells = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == 1 then
            return 1
        end
    end
end