/*
  * Generated by cppsrc.sh
  * On 2015-12-19 10:34:11,34
  * by Paweu
*/
/*--END OF HEADER BLOCK--*/

#pragma once
#ifndef ModuleRegister_H
#define ModuleRegister_H
#ifndef STARVFS_DISABLE_REGISTER

namespace StarVFS {

class Register final {
public:
	Register(StarVFS *Owner);
 	~Register();

	template<class T> 
	void RegisterContainer(const char *Name) {
		static_assert(std::is_base_of <iContainer, T>::value, "Invalid container class");
	}

	template<class T>
	void RegisterExporter(const char *Name) {
		static_assert(std::is_base_of <Exporters::iExporter, T>::value, "Invalid exporter class");
	}

	template<class T>
	void RegisterModule(const char *Name) {
		static_assert(std::is_base_of <Modules::iModule, T>::value, "Invalid module class");
	}
protected:
private: 
	StarVFS *m_Owner;
};

} //namespace StarVFS 

#endif
#endif
