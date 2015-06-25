# qsockpol
Qt Unity3D Socket Policy Server

Using example

Just add to .pro file next following
  QT       += network

main.cpp
```

#include <QCoreApplication>
#include <QScopedPointer>
#include "sockpol.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QScopedPointer<SocketPolicyServerThread> policyServer(new SocketPolicyServerThread());
    return a.exec();
}

```
