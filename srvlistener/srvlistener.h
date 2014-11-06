/*
 * Author: Sergey Kolosov <kolosov@gmail.com.com>
 */

#ifndef SRVLISTENER_H
#define SRVLISTENER_H

#include "nsCOMPtr.h"
#include "embed.h"
#include <gtk/gtk.h>

using namespace std;

enum class GeckoAnswersType {
	A_STATUS,
	A_TEXT_CONTENT,
	A_UNKNOWN
};

typedef struct tSrvGeckoGuiQ {
	gint counter;
	GAsyncQueue *queueFromGecko;
	GAsyncQueue *queueToGecko;
	GMutex *lock;
} SrvGeckoGuiQ;

typedef struct tSrvGeckoGuiMes {
	gchar *message;
	gint number = 7777777;
	GeckoAnswersType type;
} SrvGeckoGuiMes;

class SrvListener : public MozViewListener
{
public:
    SrvListener(MozView *aMozView, SrvGeckoGuiQ *aGGuiQ);

    void StatusChanged(const char* newStatus, PRUint32 statusType);
    void DocumentLoaded(bool /*aSuccess*/);
private:
    MozView *mMozView;
    SrvGeckoGuiQ *mGGuiQ;
};

#endif // SRVLISTENER_H

