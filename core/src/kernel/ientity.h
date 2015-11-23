#ifndef IENTITY_H_DEF
#define IENTITY_H_DEF

#include "softns.h"
#include <string>

SOFT_BEGIN_NAMESPACE

class IDataModel;

#define SOFT_ENTITY_METADATA(name,ns,ver)	\
  virtual const char * metaType() const {return #name;}						  \
  virtual const char * metaName() const {return #name;}   \
  virtual const char * metaNamespace() const {return #ns;} \
  virtual const char * metaVersion() const {return #ver;}

class IEntity
{
public:
  IEntity();
  explicit IEntity (std::string const &id);
  explicit IEntity (IEntity const *other);
  virtual  ~IEntity() = 0;

  virtual void        save(IDataModel *) const                = 0;
  virtual void        load(IDataModel const *)                = 0;
  static  IEntity*    create (const std::string &uuid);

  virtual std::string id()                              const;    
  virtual const char* metaType()                        const = 0;
  virtual const char* metaName()                        const = 0;
  virtual const char* metaNamespace()                   const = 0;
  virtual const char* metaVersion()                     const = 0;

private:
  class Private;
  Private *d;
};

SOFT_END_NAMESPACE

#endif // IENTITY_H_DEF
