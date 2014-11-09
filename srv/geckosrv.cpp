/*
 * Author: Sergey Kolosov <kolosov@gmail.com>
 */

//gtk
#include <gtk/gtk.h>

//gecko-embedding
#include "embed.h"
#include "EmbeddingSetup.h"
#include "srvlistener.h"

//gecko
#include <nsCOMPtr.h>
#include <nsStringAPI.h>
#include <nsIDOMWindow.h>
#include <nsIDOMDocument.h>
#include <nsIDOMElement.h>

#include <nsIDOMNodeList.h>
#include <nsIDOMNode.h>
#include <nsINode.h>

//other
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "geckosrv.h"

/*class GeckoQuestionsCommands {
public:
	static std::string Q_LOAD_URL_CMD_PREFIX = "load";
	static std::string Q_GET_TEXT_CONTENT_CMD = "get_text_content";
};*/

MozView *myMozView;
SrvListener *myMozListener;
GeckoSrvCmdParser *myCmdParser;

int create_srv_inet_socket () {
     int sockfd, portno=7070;

     struct sockaddr_in srvAddr;

	 sockfd = socket(AF_INET, SOCK_STREAM  | SOCK_NONBLOCK, 0);
	 if (sockfd < 0) {
		std::cerr << "ERROR opening socket" << std::endl;
		return -1;
	 }
	 bzero((char *) &srvAddr, sizeof(srvAddr));
	 srvAddr.sin_family = AF_INET;
	 srvAddr.sin_addr.s_addr = INADDR_ANY;
	 srvAddr.sin_port = htons(portno);
	 if (bind(sockfd, (struct sockaddr *) &srvAddr, sizeof(srvAddr)) < 0) {
		 std::cerr << "ERROR on binding" << std::endl;
		 return -1;
	 }

	 listen(sockfd,1); //only one client

	 return sockfd;
}

int acceptClient(int srvSock) {
	struct sockaddr_in clAddr;

    socklen_t clilen;
    clilen = sizeof(clAddr);

    while(1) {
    	int newsockfd = accept4(srvSock,
                (struct sockaddr *) &clAddr,
                &clilen, SOCK_NONBLOCK);
    	if (newsockfd >0) {
    		std::cout << "Got new client" << std::endl;
    		return newsockfd;
    	}
    	g_usleep(500000);
    }
}

int processSrvSocket (int clSock, GAsyncQueue *queue) {
	char buf[MAX_BUF_SIZE];
	int answrLen;

	bzero(buf, MAX_BUF_SIZE);

	ssize_t readBytes = read(clSock,(void *) buf, MAX_BUF_SIZE);
	int error= errno;
	//std::cout << "read socket, readBytes=" << to_string(readBytes) << std::endl;
	if(readBytes >= 0) {
		gchar *str = g_strdup((const gchar*)buf);
		std::cout << "Here is the message: >>" << str << "<<" << std::endl;
		g_async_queue_push (queue,(gpointer)str);

	} else if (readBytes < 0 && error == EAGAIN ) {
		;;
		//nothing new data here
	} else {
		;;
		//some error
	}
}

int processGuiMesFromGecko (int clSock, GAsyncQueue *queue) {
	gpointer data = g_async_queue_try_pop (queue);
	if(data) {
		g_print ("1111-----------\n");
		//g_mutex_lock(&(mypSrvQ->lock));
		//SrvGeckoGuiMes b;
		SrvGeckoGuiMes *myMes = (SrvGeckoGuiMes*)data;
		char *str=myMes->message;

		//ssize_t wroteBytes
		write(clSock,(const void *) str, (size_t)(strlen(str)+1));
		g_print("\n>>>GOT MESSAGE FROM GECKO: %s :%d<<<\n", myMes->message, myMes->number);

		//g_mutex_unlock(&(mypSrvQ->lock));
	}
}

static gpointer
srvThreadFunc( gpointer data )
{
	std::cout << "Gecko Gui thread is starting" << std::endl;
	int srvSock = create_srv_inet_socket();
	if (srvSock < 0) {
		std::cout << "Gecko Gui thread is stopping" << std::endl;
		return NULL;
	}

	int clSock = acceptClient(srvSock);
	if (clSock < 0) {
		std::cout << "Gecko Gui thread is stopping" << std::endl;
		return NULL;
	}

	SrvGeckoGuiQ *mypSrvQ = (SrvGeckoGuiQ*)data;

    GAsyncQueue *queueFrom = mypSrvQ->queueFromGecko;
    GAsyncQueue *queueTo = mypSrvQ->queueToGecko;
    g_async_queue_ref (queueFrom);
    g_async_queue_ref (queueTo);

	while(1) {
		processSrvSocket(clSock, queueTo);
		processGuiMesFromGecko(clSock, queueFrom);
		g_usleep(500000);
	}

	g_async_queue_unref(queueFrom);
	g_async_queue_unref(queueTo);

	return NULL;
}

bool GeckoSrvCmdParser::ParseCmd(std::string aCmd)
{
	//TODO check for new lines

	if((aCmd.back()=='\r') || (aCmd.back()=='\n')) aCmd.pop_back();
	if((aCmd.back()=='\r') || (aCmd.back()=='\n')) aCmd.pop_back();


	if(aCmd.substr(0,Q_LOAD_URL_CMD_PREFIX.length()) == Q_LOAD_URL_CMD_PREFIX) {
		std::string url = aCmd.substr(Q_LOAD_URL_CMD_PREFIX.length()+1, aCmd.length());
		//g_print ("\nLOAD_URL:%s\n", url.c_str());
		myMozView->LoadURI(url.c_str());

	}
	else if (aCmd == Q_GET_TEXT_CONTENT_CMD) {
		//g_print ("\nGET_TEXT_CONTENT\n");
		GetAllTextContent();
	}
	return true;
}


std::string GeckoSrvCmdParser::GetAllTextContent()
{
	//std::string textContent="Simple text content";

	cout << "START TEXTCONTENT" << endl;
	nsCOMPtr<nsIDOMWindow> myDomWindow = myMozView->GetDOMWindow();

	nsCOMPtr<nsIDOMDocument> myDomDocument;

	myDomWindow->GetDocument(getter_AddRefs(myDomDocument));

	nsCOMPtr<nsIDOMElement> myDomElement;
	myDomDocument->GetDocumentElement(getter_AddRefs(myDomElement));

	nsString text;
	std::string textContent;

	myDomElement->GetTextContent(text);

	textContent = ToNewUTF8String(text);
	cout << textContent << endl;

	return textContent;
}

static gboolean
geckoGuiController( gpointer data )
{
    SrvGeckoGuiQ *mypSrvQ = (SrvGeckoGuiQ*)data;

    GAsyncQueue *queue = mypSrvQ->queueToGecko;
    g_async_queue_ref (queue);

    gpointer message = g_async_queue_try_pop (queue);
	if(message) {
		g_print ("\n1111-----------\n");
		g_print ("Query size=%d\n",g_async_queue_length (queue));
		//g_mutex_lock(&(mypSrvQ->lock));
		//SrvGeckoGuiMes b;
		//SrvGeckoGuiMes *myMes = (SrvGeckoGuiMes*)data;
		char *str=(char*)message;

		//myMozView->LoadURI("http://www.mozilla.org");
		//ssize_t wroteBytes
		//write(clSock,(const void *) str, (size_t)(strlen(str)+1));
		g_print("\n>>>GOT MESSAGE: %s<<<\n", str);
		std::string cmd(str);
		myCmdParser->ParseCmd(cmd);

		g_free(str);
		//g_mutex_unlock(&(mypSrvQ->lock));
	}

    g_async_queue_unref(queue);

    return( TRUE );
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *g_viewport;
    gint w=1024,h=768;

    GThread   *geckoGuiThread;
    GThread   *srvThread;
    GError    *error = NULL;

    if( ! g_thread_supported() )
            g_thread_init( NULL );


    gtk_init (&argc, &argv);

    myMozView = new MozView();
    
    myCmdParser = new GeckoSrvCmdParser();

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), w,h);
    g_signal_connect(window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect( G_OBJECT( window ), "destroy",
                         G_CALLBACK( gtk_main_quit ), NULL );

    GtkWidget *box = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    g_viewport = gtk_viewport_new(NULL, NULL);

    gtk_box_pack_start(GTK_BOX(box), g_viewport, TRUE, TRUE, 0);

    int res = myMozView->CreateBrowser(g_viewport, 0, 0, w-50, h-50, 0);

    //init queue
    SrvGeckoGuiQ *mySrvQ = new SrvGeckoGuiQ;
	mySrvQ->queueFromGecko = g_async_queue_new();
	mySrvQ->queueToGecko = g_async_queue_new();
	mySrvQ->counter = 0;
	mySrvQ->lock = new GMutex;
	//init mutex
	g_mutex_init(mySrvQ->lock);

    myMozListener = new SrvListener(myMozView, mySrvQ);

    myMozView->SetListener(myMozListener);

    gdk_threads_add_timeout( 500, geckoGuiController, (gpointer)mySrvQ );

    srvThread = g_thread_create( srvThreadFunc, (gpointer)mySrvQ, FALSE, &error);
    if( ! srvThread )
    {
        g_print( "Error: %s\n", error->message );
        return( -1 );
    }


    gtk_widget_show_all(window);

    //if(argc < 2)
    //    	myMozView->LoadURI("http://www.mozilla.org");
    //    else myMozView->LoadURI(argv[1]);


    gtk_main ();
    
    return 0;
}

