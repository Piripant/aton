SCRIPT_TYPE = "CA"
TAGS = { 
    {0, "Resting"}, 
    {1, "Refractoring"},
    {2, "Exited"} 
}

NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATES_NUM = 3

resting = 0
refractory = 1
excited = 2

function get_next_state (neighbors, state)
    if state == refractory then
        return resting
    elseif state == excited then
        return refractory
    end

    for i, neighbor in pairs(neighbors) do
        if neighbor == excited then
            return excited
        end
    end

    return resting
end