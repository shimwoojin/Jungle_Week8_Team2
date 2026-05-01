#pragma once

namespace sol
{
	class state;
}

void RegisterLuaBindings(sol::state& Lua);

void RegisterFVectorBinding(sol::state& Lua);
void RegisterGameObjectBinding(sol::state& Lua);

void RegisterShapeComponentBinding(sol::state& Lua);
void RegisterBoxComponentBinding(sol::state& Lua);
void RegisterSphereComponentBinding(sol::state& Lua);
void RegisterCapsuleComponentBinding(sol::state& Lua);

void RegisterDelegateBinding(sol::state& Lua);
