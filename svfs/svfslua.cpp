/*
  * Generated by cppsrc.sh
  * On 
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#include "luainterface.h"
#include "svfslua.h"
#include <core/nStarVFS.h>

static void InstallStarVFS(lua_State *lua) {

}

static void InstallFileHandle(lua_State *lua) {
	using ::StarVFS::FileHandle;
	struct FileHandleHelper {
		int IsDirectory() { return ((FileHandle*)this)->IsDirectory() ? 1 : 0; }
		int IsSymlink() { return ((FileHandle*)this)->IsSymlink() ? 1 : 0; }
		int IsHandleValid() { return ((FileHandle*)this)->IsHandleValid() ? 1 : 0; }

		int EnumerateChildren(lua_State *Lua) {
			auto *h = (FileHandle*)this;
			if (!h) return 0;
			int c = 0;
			lua_createtable(Lua, 0, 0);
			bool succ = h->EnumerateChildren([Lua, &c](::StarVFS::FileID fid) {
				++c;
				lua_pushinteger(Lua, c);
				lua_pushinteger(Lua, fid);
				lua_settable(Lua, -3);
				return true;
			});
			if (!succ) {
				lua_pop(Lua, 1);
				lua_pushnil(Lua);
				return 1;
			}
			return 1;
		}
	};

	luabridge::getGlobalNamespace(lua)
		.beginNamespace("api")
			.beginClass<FileHandle>("FileHandle")
			//.addFunction("Clone", &FileHandle::Clone)
			.addFunction("GetSize", &FileHandle::GetSize)
			//.addFunction("GetFullPath", &FileHandle::GetFullPath)
			//.addFunction("GetRWMode", &FileHandle::GetRWMode)
			.addFunction("IsDirectory", (int(FileHandle::*)())&FileHandleHelper::IsDirectory)
			.addFunction("IsSymlink", (int(FileHandle::*)())&FileHandleHelper::IsSymlink)
			.addFunction("IsValid", (int(FileHandle::*)())&FileHandleHelper::IsHandleValid)
			//.addFunction("Close", &FileHandle::Close)
			.addCFunction("GetChildren", (int(FileHandle::*)(lua_State *))&FileHandleHelper::EnumerateChildren)
			//bool GetFileData(CharTable &data) const;
			.endClass()
		.endNamespace()
	;
}

static void InstallAttribMap(lua_State *lua) {
	using ::StarVFS::AttributeMap;

	struct Helper {
		int GetAttribs(lua_State *l) {
			auto *a = (AttributeMap*)this;
			auto v = a->GetAttributeNames();

			lua_createtable(l, v.size(), 0);

			for (size_t i = 0; i < v.size(); ++i) {
				lua_pushinteger(l, i);
				lua_pushstring(l, v[i].c_str());
				lua_settable(l, -3);
			}

			return 1;
		}

		int Set(const char *name, const char *value) {
			if (!name || !value)
				return 0;
			auto *a = (AttributeMap*)this;
			auto m = &a->GetAttributeMap();

			return m->Set(a, name, value) ? 1 : 0;
		}

		int Get(lua_State *l) {
			const char *name = lua_tostring(l, -1);
			if (!name) {
				lua_pushnil(l);
				return 1;
			}

			auto *a = (AttributeMap*)this;
			auto m = &a->GetAttributeMap();
			StarVFS::String v;
			if (!m->Get(a, name, v)) {
				lua_pushnil(l);
				return 1;
			}
			lua_pushstring(l, v.c_str());
			return 1;
		}
	};

	luabridge::getGlobalNamespace(lua)
		.beginNamespace("api")
			.beginClass<AttributeMap>("AttributeMap")
				.addCFunction("GetAttributeList", (int(AttributeMap::*)(lua_State *))&Helper::GetAttribs)
				.addCFunction("GetAttribute", (int(AttributeMap::*)(lua_State *))&Helper::Get)
				.addFunction("SetAttribute", (int(AttributeMap::*)(const char *, const char *))&Helper::Set)
			.endClass()
		.endNamespace()
		;
}

static void InstalliContainer(lua_State *lua) {
	using ::StarVFS::AttributeMap;
	using ::StarVFS::iContainer;
	luabridge::getGlobalNamespace(lua)
		.beginNamespace("api")
			.deriveClass<iContainer, AttributeMap>("iContainer")
			.endClass()
		.endNamespace()
		;
	//.beginClass<::StarVFS::iContainer>("iContainer")
	//.endClass()
}

static void InstalliModule(lua_State *lua) {
	using ::StarVFS::AttributeMap;
	using ::StarVFS::Modules::iModule;
	luabridge::getGlobalNamespace(lua)
		.beginNamespace("api")
			.deriveClass<iModule, AttributeMap>("iModule")
			.endClass()
		.endNamespace()
		;
}

static void InstalliExporter(lua_State *lua) {
	using ::StarVFS::AttributeMap;
	using ::StarVFS::Exporters::iExporter;
	luabridge::getGlobalNamespace(lua)
		.beginNamespace("api")
			.deriveClass<iExporter, AttributeMap>("iExporter")
			.endClass()
		.endNamespace()
		;
}

//-------------------------------------------------------------------------------------------------

void svfslua::Install(lua_State *lua) {
	try {
		InstallStarVFS(lua);
		InstallFileHandle(lua);

		InstallAttribMap(lua);
		InstalliContainer(lua);
		InstalliModule(lua);
		InstalliExporter(lua);
	}
	catch (...) {
		printf("Failed to install svfs lua interface!\n");
		exit(1);
	}
}
