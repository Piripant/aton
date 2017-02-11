NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1} }
STATES_NUM = 2

alive = 1
dead = 0

function get_next_state (neighbors, state)
    local alive_neigh = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == alive then
            alive_neigh = alive_neigh + 1
        end
    end

    if state == alive then
        if alive_neigh < 2 then
            return dead
        elseif alive_neigh == 2 or alive_neigh == 3 then
            return alive
        elseif alive_neigh > 3 then
            return dead
        end
    elseif state == dead then
        if alive_neigh == 3 then
            return alive
        end
    end
end