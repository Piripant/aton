NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATE_COLORS = { {255, 255, 255}, {0, 255, 255} }
STATES_NUM = 2

function get_next_state (neighbors, state)
    local alive_cells = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == 1 then
            alive_cells = alive_cells + 1
        end
    end

    if alive_cells < 2 then
        return 0
    elseif alive_cells > 3 then
            return 0
    elseif alive_cells == 3 then
        return 1
    end
end