-- UIExample.lua
-- 원하는 게임 매니저 Lua 파일에서 Include 하거나 필요한 부분만 옮겨 쓰세요.

local score = 0
local best = 0
local coins = 0
local lane = 1
local combo = 1

local function RefreshHud()
    UI.SetScore(score)
    UI.SetBestScore(best)
    UI.SetCoins(coins)
    UI.SetLane(lane)
    UI.SetCombo(combo)
end

function StartRun()
    score = 0
    coins = 0
    lane = 1
    combo = 1

    UI.ResetRun()
    UI.SetStatus("앞으로 이동해서 도로를 건너세요!")
    RefreshHud()
end

function EndRun()
    if score > best then
        best = score
    end

    UI.SetBestScore(best)
    UI.ShowGameOver(score, best)
end

function AddScore(amount)
    score = score + amount
    if score > best then
        best = score
    end
    RefreshHud()
end

function AddCoin(amount)
    coins = coins + amount
    RefreshHud()
end

function SetLane(value)
    lane = value
    RefreshHud()
end

function SetCombo(value)
    combo = value
    RefreshHud()
end

UI.SetEventHandler(function(eventName)
    if eventName == "start" then
        StartRun()
    elseif eventName == "restart" then
        StartRun()
    elseif eventName == "main_menu" then
        UI.ShowHUD(false)
        UI.ShowGameOver(false)
        UI.ShowIntro(true)
    elseif eventName == "continue" then
        UI.SetStatus("계속 진행하세요!")
    elseif eventName == "exit" then
        -- C++ 쪽 기본 종료 콜백도 같이 호출됩니다.
    end
end)

UI.ShowIntro(true)
UI.ShowHUD(false)
