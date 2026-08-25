#ifndef NOKILIB_MANAGER
#define NOKILIB_MANAGER

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "lib.hpp"
#include "libinitializer.hpp"

class LibraryManager
{
public:

    explicit LibraryManager(RuntimeObjectManager& objectManager);

    void registerLibrary(const Library& library); //regista uma biblioteca

    Library*        findLibrary(const std::string& name); //encontra a lib e permite fazer a consulta das funçoes respetivas
    const Library*  findLibrary(const std::string& name) const;

    bool exists(const std::string& name) const; //se existir a lib devolve true se nao da exit

    bool importLibrary(const std::string& name);
    bool isImported(const std::string& name) const;

private:

    RuntimeObjectManager& objectManager;
    std::unordered_map<std::string, Library> libraries;
    std::unordered_set<std::string> importedLibraries;
    std::unordered_map< std::string,std::unique_ptr<LibraryInitializer> > initializers;
};

#endif 