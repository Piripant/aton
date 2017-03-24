SCRIPT_TYPE = "CA"

NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1} }
STATES_NUM = 2

alive = 1
dead = 0

function get_next_state (neighbors, state)
    local alive_cells = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == 1 then
            alive_cells = alive_cells + 1
        end
    end

    if alive_cells == 3 or alive_cells == 6 or alive_cells == 7 or alive_cells == 8 then
        return alive
    else
        return dead
    end
end