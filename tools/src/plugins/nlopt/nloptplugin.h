#include <QObject>
#include "isoftplugin.h"

class QScriptEngine;
class NLoptPlugin : public QObject
		, public ISoftPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.sintef.soft/ISoftPlugin/0.1")
  Q_INTERFACES(ISoftPlugin)
  
  public:
  virtual ~NLoptPlugin();
  virtual void registerPlugin(QScriptEngine *engine) override;
  
};
