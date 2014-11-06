/*
 * Author: Sergey Kolosov <kolosov@gmail.com.com>
 */

#include "srvlistener.h"
#include "geckoembed_config.h"

#include <iostream>
#include <string>
#include <iomanip>

SrvListener::SrvListener(MozView *aMozView, SrvGeckoGuiQ *aGGuiQ)
{
    mMozView=aMozView;
    mGGuiQ=aGGuiQ;
    //g_mutex_init (&mGeckoGuiMutex);
}

void SrvListener::StatusChanged(const char* newStatus, PRUint32 statusType)
{
	std::cout << "Status changed to " << string(newStatus) << std::endl;

	SrvGeckoGuiMes *myMessage = new SrvGeckoGuiMes;
	gchar *str= g_strdup("Status changed\n");
	//int mes= 999999;
	myMessage->message = str;
	myMessage->number = 999999;
	myMessage->type = GeckoAnswersType::A_STATUS;
	g_print("SEND:%s %d\n", myMessage->message, myMessage->number);
	//g_mutex_lock (&(mGGuiQ->lock));
	//g_async_queue_push (mGGuiQ->queue,&myMessage );
	g_async_queue_push (mGGuiQ->queueFromGecko,(gpointer)myMessage);
	//g_mutex_unlock (&(mGGuiQ->lock));
	g_print("%s\n","EXIT");
}

void SrvListener::DocumentLoaded(bool aSuccess)
{
	SrvGeckoGuiMes *myMessage = new SrvGeckoGuiMes;
	if(aSuccess ) {
		std::cout << "Document loaded successful" << std::endl;
		myMessage->type = GeckoAnswersType::A_STATUS;
		myMessage->message = g_strdup ("Document Loaded successful\n");
		//g_mutex_lock (&(mGGuiQ->lock));
		g_async_queue_push (mGGuiQ->queueFromGecko,(gpointer)myMessage );
		//g_mutex_unlock (&(mGGuiQ->lock));
	} else {
		std::cout << "Document loaded unsuccessful" << std::endl;
		myMessage->type = GeckoAnswersType::A_STATUS;
		myMessage->message = g_strdup("Document Loaded unsuccessful\n");
		//g_mutex_lock (&(mGGuiQ->lock));
		g_async_queue_push (mGGuiQ->queueFromGecko,(gpointer)myMessage );
		//g_mutex_unlock (&(mGGuiQ->lock));
	}
}

