-- Generated from async.nvim by scripts/vendor_nvim.lua.
-- This file is intended for vendoring into the Neovim codebase.

-- LuaLS cannot model the generic annotations used by this vendored implementation.
---@diagnostic disable: no-unknown, undefined-doc-name, luadoc-miss-symbol, missing-return, missing-return-value, param-type-mismatch, return-type-mismatch, redundant-return-value, undefined-field, need-check-nil, await-in-sync

local safe_pcall = pcall
local safe_running = coroutine.running
local ok, coxpcall = pcall(require, 'coxpcall')
if ok and type(coxpcall) == 'table' and type(coxpcall.pcall) == 'function' then
  safe_pcall = coxpcall.pcall
  safe_running = coxpcall.running or safe_running
end

local M = {}

M._maxint = 2 ^ 32 - 1
M.pcall = safe_pcall
M.running = safe_running

--- @nodoc
function M.is_callable(obj)
  local t = type(obj)
  if t == 'function' then
    return true
  elseif t == 'table' then
    local mt = getmetatable(obj)
    return mt and type(mt.__call) == 'function'
  end
  return false
end

--- @nodoc
function M.validate(name, value, expected_type, optional)
  if optional and value == nil then
    return
  end

  local actual_type = type(value)
  local valid = false

  if expected_type == 'callable' then
    valid = M.is_callable(value)
  elseif expected_type == 'table' then
    valid = actual_type == 'table'
  elseif expected_type == 'number' then
    valid = actual_type == 'number'
  elseif expected_type == 'boolean' then
    valid = actual_type == 'boolean'
  else
    error(string.format('validate: unsupported type "%s"', expected_type), 2)
  end

  if not valid then
    local got = expected_type == 'callable' and (M.is_callable(value) and 'callable' or actual_type)
      or actual_type
    error(string.format('%s: expected %s, got %s', name, expected_type, got), 2)
  end
end

return M
